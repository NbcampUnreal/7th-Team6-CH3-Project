// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/SanzoGameState.h"

#include "Blueprint/UserWidget.h"
#include "Character/SanzoCharacter.h"
#include "Character/SanzoPlayerController.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

#pragma region UI

ASanzoGameState::ASanzoGameState()
{
}

void ASanzoGameState::BeginPlay()
{
	Super::BeginPlay();

	StartStage();

	GetWorldTimerManager().SetTimer(
		HUDUpdateTimerHandle,
		this,
		&ASanzoGameState::UpdateHUD,
		0.1f,
		true
	);
}

void ASanzoGameState::UpdateHUD()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASanzoPlayerController* SanzoPlayerController = Cast<ASanzoPlayerController>(PlayerController))
		{
			if (UUserWidget* HUDWidget = SanzoPlayerController->GetHUDWidget())
			{
				if (ASanzoCharacter* SanzoCharacter = Cast<ASanzoCharacter>(SanzoPlayerController->GetPawn()))
				{
					if (UProgressBar* ExpBar = Cast<UProgressBar>(HUDWidget->GetWidgetFromName("ExpBar")))
					{
						//캐릭터에서 경험치 정보 가져옴
						//float Percent = SanzoCharacter->GetExp() / SanzoCharacter->GetMaxExp();
						//ExpBar->SetPercent(Percent);

						//임시 테스트 코드
						ExpBar->SetPercent(0.5);
					}
					if (UProgressBar* HealthBar = Cast<UProgressBar>(HUDWidget->GetWidgetFromName("HealthBar")))
					{
						//캐릭터에서 체력 정보 가져옴
						//float Percent = SanzoCharacter->GetHealth() / SanzoCharacter->GetMaxHealth();
						//HealthBar->SetPercent(Percent);

						//임시 테스트 코드
						HealthBar->SetPercent(0.8);
					}
					if (UProgressBar* StaminaBar = Cast<UProgressBar>(HUDWidget->GetWidgetFromName("StaminaBar")))
					{
						//캐릭터에서 스테미나 정보 가져옴
						//float Percent = SanzoCharacter->GetStamina() / SanzoCharacter->GetMaxStamina();
						//StaminaBar->SetPercent(Percent);

						//임시 테스트 코드
						StaminaBar->SetPercent(0.5);
					}
					if (UTextBlock* LevelText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName("LevelText")))
					{
						//캐릭터에서 레벨 정보 가져옴
						//LevelText->SetText(FText::FromString(FString::Printf(TEXT("%d"), SanzoPlayerController->GetLevel())));

						//임시 테스트 코드
						LevelText->SetText(FText::FromString(FString::Printf(TEXT("1"))));
					}
					if (UTextBlock* AmmoCountText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName("AmmoCountText")))
					{
						//캐릭터에서 총기 총알 정보 가져옴
						//AmmoCountText->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), SanzoPlayerController->Ammo(),SanzoPlayerController->MaxAmmo())));

						//임시 테스트 코드
						AmmoCountText->SetText(FText::FromString(FString::Printf(TEXT("30/30"))));
					}
					if (UImage* WeaponImage = Cast<UImage>(HUDWidget->GetWidgetFromName("WeaponImage")))
					{
						//이미지 설정 코드 구현
					}
				}
				/*
				 *Stage 정보 등록 필요
				 *Stage Progress Bar 등록 필요
				 */
			}
		}
	}
}

//테스트용 임시 함수
void ASanzoGameState::StartStage()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASanzoPlayerController* SanzoPlayerController = Cast<ASanzoPlayerController>(PlayerController))
		{
			SanzoPlayerController->ShowGameHUD();
		}
	}
}
#pragma endregion 이준로
