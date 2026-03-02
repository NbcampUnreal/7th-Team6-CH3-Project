// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SanzoHUDWidget.h"

#include "Character/Components/SanzoEquipmentComponent.h"
#include "Character/Components/SanzoStatComponent.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/ProgressBar.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Core/SanzoGameState.h"
#include "UI/SanzoItemNotificationWidget.h"
#include "UI/SanzoStageInfoWidget.h"
#include "Weapon/SanzoBow.h"

void USanzoHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (APawn* PlayerCharacter = GetOwningPlayerPawn())
	{
		//StatComponent 추출
		USanzoStatComponent* StatComponent = PlayerCharacter->FindComponentByClass<USanzoStatComponent>();
		if (StatComponent)
		{
			//Delegate 등록
			//this : 대상 객체, &USanzoHUDWidget::HandleStatChanged: 실행할 함수
			StatComponent->OnStatChanged.AddDynamic(this, &USanzoHUDWidget::HandleStatChanged);
			StatComponent->OnExhaustedChanged.AddDynamic(this, &USanzoHUDWidget::HandleStaminaColorChange);
		}
		
		USanzoEquipmentComponent* EquipmentComponent = PlayerCharacter->FindComponentByClass<USanzoEquipmentComponent>();
		if (EquipmentComponent)
		{
			EquipmentComponent->OnAmmoChanged.AddDynamic(this, &USanzoHUDWidget::HandleAmmoChanged);
			EquipmentComponent->OnSwapped.AddDynamic(this, &USanzoHUDWidget::HandleWeaponSwapped);
			EquipmentComponent->OnAnyWeaponHitEnemy.AddDynamic(this, &USanzoHUDWidget::HandleEnemyHitAnim);
			EquipmentComponent->OnItemPickedUp.AddDynamic(this, &USanzoHUDWidget::HandleItemNotification);
		}
		
		if (BowAimProgressBar)
		{
			BowAimProgressBarDynamic = BowAimProgressBar->GetDynamicMaterial();
			
			if (BowAimProgressBarDynamic)
			{
				BowAimProgressBarDynamic->SetScalarParameterValue(TEXT("Percentage"),0.0f);
			}
		}
	}

	if (ASanzoGameState* GameState = GetWorld()->GetGameState<ASanzoGameState>())
	{
		ESanzoStageType CurrentStageType = GameState->CurrentStageType;
		
		if (StageInfoWidget)
		{
			StageInfoWidget->SetStageInfo(CurrentStageType);
		}
	}
}

void USanzoHUDWidget::HandleStatChanged(const FSanzoStatData& Data)
{
	if (HealthBar)
	{
		HandleHealthBarData(Data.MaxHealth,Data.CurrentHealth,Data.HealthPercent);
	}
	if (StaminaBar)
	{
		StaminaBar->SetPercent(Data.StaminaPercent);
	}
	if (ExpBar)
	{
		ExpBar->SetPercent(Data.ExpPercent);
	}
	if (LevelText)
	{
		LevelText->SetText(FText::AsNumber(Data.CurrentLevel));
	}
}

void USanzoHUDWidget::HandleHealthBarData(float MaxHealth, float CurrentHealth, float NewPercent)
{
	if (HealthInfoText)
	{
		HealthInfoText->SetText(FText::Format(
			FText::FromString(TEXT("{0} / {1}"))
			,FText::AsNumber(CurrentHealth)
			,FText::AsNumber(MaxHealth)));
	}
	
	if (HealthBarSizeBox)
	{
		HealthBarSizeBox->SetWidthOverride(MaxHealth * SizePerValue);
	}
	
	if (HealthBar)
	{
		HealthBar->SetPercent(NewPercent);
	}
}

void USanzoHUDWidget::HandleStaminaBarData(float MaxStamina, float CurrentStamina, float NewPercent)
{
	if (StaminaInfoText)
	{
		StaminaInfoText->SetText(FText::Format(
			FText::FromString(TEXT("{0} / {1}"))
			,FText::AsNumber(CurrentStamina)
			,FText::AsNumber(MaxStamina)));
	}
	
	if (StaminaBarSizeBox)
	{
		StaminaBarSizeBox->SetWidthOverride(MaxStamina * SizePerValue);
	}
	
	if (StaminaBar)
	{
		StaminaBar->SetPercent(NewPercent);
	}
}

void USanzoHUDWidget::HandleStaminaColorChange(bool bNewExhausted)
{
	bIsExhausted = bNewExhausted;
	if (bIsExhausted)
	{
		ExhaustionStartTime = GetWorld()->GetTimeSeconds();
		if (StaminaBar)
		{
			StaminaBar->SetFillColorAndOpacity(ExhaustedColor);
		}
	}
}

void USanzoHUDWidget::UpdateStaminaColor(float DeltaTime)
{
	if (!StaminaBar) return;
	
	float ElapsedTime = GetWorld()->GetTimeSeconds() - ExhaustionStartTime;
	
	float Percent = FMath::Clamp(ElapsedTime/ExhaustionDuration, 0.f, 1.f);
	
	FLinearColor CurrentColor = FLinearColor::LerpUsingHSV(ExhaustedColor, NormalStaminaColor, Percent);
	
	StaminaBar->SetFillColorAndOpacity(CurrentColor);
	
	if (Percent >= 1.0f)
	{
		bIsExhausted = false;
		StaminaBar->SetFillColorAndOpacity(NormalStaminaColor);
	}
}

void USanzoHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (bIsExhausted)
	{
		UpdateStaminaColor(InDeltaTime);
	}
}


void USanzoHUDWidget::HandleAmmoChanged(FText NewAmmoText)
{
	GunAmmoCount->SetText(NewAmmoText);
}

void USanzoHUDWidget::HandleWeaponSwapped(int32 CurrentWeaponIndex)
{
	bool bIsGunMain = (CurrentWeaponIndex == 0);
	
	UCanvasPanelSlot* GunSlot = Cast<UCanvasPanelSlot>(GunInfoOverlay->Slot);
	UCanvasPanelSlot* BowSlot = Cast<UCanvasPanelSlot>(BowInfoOverlay->Slot);
	
	if (GunSlot && BowSlot)
	{
		if (bIsGunMain)
		{
			GunSlot->SetZOrder(1);
			BowSlot->SetZOrder(0);
		
			PlayAnimation(GunInfoSwapBackAnim, 0.0f, 1, EUMGSequencePlayMode::Reverse);
		}
		else
		{
			GunSlot->SetZOrder(0);
			BowSlot->SetZOrder(1);
			
			PlayAnimation(GunInfoSwapBackAnim, 0.0f, 1, EUMGSequencePlayMode::Forward);
		}
	}
	
	if (APawn* PlayerCharacter = GetOwningPlayerPawn())
	{
		USanzoEquipmentComponent* EquipmentComponent = PlayerCharacter->FindComponentByClass<USanzoEquipmentComponent>();
		if (EquipmentComponent)
		{
			ASanzoWeaponBase* CurrentWeapon = EquipmentComponent->GetCurrentWeapon();
			if (ASanzoBow* Bow = Cast<ASanzoBow>(CurrentWeapon))
			{
				Bow->OnChargePercentChanged.RemoveAll(this);
				Bow->OnChargePercentChanged.AddDynamic(this, &USanzoHUDWidget::UpdateBowChargingProgress);
			}
			else
			{
				UpdateBowChargingProgress(0.0f);
			}
		}
	}
}

void USanzoHUDWidget::UpdateBowChargingProgress(float NewPercent)
{
	if (BowAimProgressBarDynamic)
	{
		BowAimProgressBarDynamic->SetScalarParameterValue(TEXT("Percentage"), NewPercent);
	}
	
	if (BowAimProgressBar)
	{
		bool bIsVisible = NewPercent > 0.0f;
		ESlateVisibility BowAimProgressBarVisibility = bIsVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
		BowAimProgressBar->SetVisibility(BowAimProgressBarVisibility);
		
		if (bIsVisible)
		{
			FLinearColor StartColor = FLinearColor::White;
			FLinearColor EndColor = FLinearColor::Green;

			// NewPercent(0~1)에 따라 두 색상 사이의 값을 계산
			FLinearColor CurrentColor = FLinearColor::LerpUsingHSV(StartColor, EndColor, NewPercent);

			// 위젯의 색조(Tint)를 설정
			BowAimProgressBar->SetColorAndOpacity(CurrentColor);
		}
	}
	
}

void USanzoHUDWidget::HandleEnemyHitAnim()
{
	if (HitEffectAnim)
	{
		PlayAnimation(HitEffectAnim);
	}
}
#pragma region ItemNotification

void USanzoHUDWidget::HandleItemNotification(FName ItemType, int32 Amount)
{
	if (!ItemNotificationClass || !NotificationContainer) return;
	
	USanzoItemNotificationWidget* NewNotify = CreateWidget<USanzoItemNotificationWidget>(GetOwningPlayer(), ItemNotificationClass);
	if (NewNotify)
	{
		NewNotify->SetNotify(ItemType, Amount);
		
		NotificationContainer->AddChildToVerticalBox(NewNotify);
		
	}
}

#pragma endregion 이준로