// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SanzoUpgradeButtonWidget.h"

#include "Common/SanzoLog.h"
#include "Components/AudioComponent.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Core/UpgradeSystem/SanzoUpgradeSubsystem.h"
#include "Kismet/GameplayStatics.h"

void USanzoUpgradeButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (UpgradeButton)
	{
		UpgradeButton->OnClicked.AddDynamic(this, &ThisClass::HandleButtonClicked);		
	}
}

void USanzoUpgradeButtonWidget::SetUpgradeButton(const FUpgradeOption& InputOption)
{
	CachedOption = InputOption;
	
	if (UpgradeBackground)
	{
		UpgradeBackground->SetBrushFromTexture(GetTextureByRarity(InputOption.Rarity));
	}
	
	if (UpgradeText && !InputOption.DisplayName.IsEmpty())
	{
		FText DisplayText = FText::Format(FText::FromString("{0}{1}"),
			InputOption.DisplayName,
			InputOption.Value == 0 ? FText::FromString("") : (FText::Format(FText::FromString(" + {0}"), FText::AsNumber(InputOption.Value)))
			);
		UpgradeText->SetText(DisplayText);
	}
	
	if (UpgradeIcon)
	{
		if (InputOption.IconTexture.IsNull())
		{
			UpgradeIcon->SetVisibility(ESlateVisibility::Hidden);
		}
		UpgradeIcon->SetBrushFromTexture(InputOption.IconTexture.LoadSynchronous());
	}
	
	SetSoundByRarity(InputOption.Rarity);
}

void USanzoUpgradeButtonWidget::HandleButtonClicked()
{
	OnUpgradeButtonClicked.Broadcast(CachedOption);
}

UTexture2D* USanzoUpgradeButtonWidget::GetTextureByRarity(EUpgradeRarity Rarity)
{
	UE_LOG(LogLJR, Warning, TEXT("레어리티 선택 들어옴"));
	if (RarityTextures.Contains(Rarity))
	{
		UE_LOG(LogLJR, Warning, TEXT("레어리티 선택됨 들어옴"));
		return RarityTextures[Rarity];
	}
	return nullptr;
}

void USanzoUpgradeButtonWidget::PlaySoundForDuration(float Duration)
{
	if (CurrentSound)
	{
		StopCurrentSound();

		CurrentAudioComponent = UGameplayStatics::SpawnSound2D(this, CurrentSound);
		if (CurrentAudioComponent)
		{
			CurrentAudioComponent->bIsUISound = true;

			CurrentAudioComponent->SetTickableWhenPaused(true);

			CurrentAudioComponent->Play();
			
			SoundStopTargetRealTime = GetWorld()->GetRealTimeSeconds()+Duration;
			bIsSoundDurationEnded = true;
			
		}
	}
}

void USanzoUpgradeButtonWidget::StopCurrentSound()
{
	bIsSoundDurationEnded = false;
	
	if (CurrentAudioComponent && CurrentAudioComponent->IsPlaying())
	{
		CurrentAudioComponent->FadeOut(0.5f,0);
		CurrentAudioComponent = nullptr;
	}
}

void USanzoUpgradeButtonWidget::SetSoundByRarity(EUpgradeRarity Rarity)
{
	if (RaritySounds.Contains(Rarity))
	{
		CurrentSound = RaritySounds[Rarity];
	}
}

void USanzoUpgradeButtonWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (bIsSoundDurationEnded)
	{
		if (GetWorld()->RealTimeSeconds >= SoundStopTargetRealTime)
		{
			StopCurrentSound();
		}
	}
}

