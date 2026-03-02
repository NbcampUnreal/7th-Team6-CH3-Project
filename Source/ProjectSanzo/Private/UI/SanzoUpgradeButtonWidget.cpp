// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SanzoUpgradeButtonWidget.h"
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
		SetUpgradeButtonText(InputOption.DisplayName, InputOption.ModifierType, InputOption.Value);
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

void USanzoUpgradeButtonWidget::SetUpgradeButtonText(FText InDisplayName, EStatModifierType InModifierType, float InValue)
{
	FText DisplayFormat;
	if (InValue == 0)
	{
		DisplayFormat = FText::FromString("");
		UpgradeText->SetText(DisplayFormat);
		return;
	}
	
	switch (InModifierType)
	{
	case EStatModifierType::FlatPlus:
		DisplayFormat = FText::Format(FText::FromString("{0}{1}"),
			InDisplayName,
			(FText::Format(FText::FromString(" + {0}"), FText::AsNumber(InValue)))
			);
		UpgradeText->SetText(DisplayFormat);
		break;
	case EStatModifierType::FlatMinus:
		DisplayFormat = FText::Format(FText::FromString("{0}{1}"),
			InDisplayName,
			(FText::Format(FText::FromString(" - {0}"), FText::AsNumber(InValue)))
			);
		UpgradeText->SetText(DisplayFormat);
		break;
	case EStatModifierType::PercentPlus:
		DisplayFormat = FText::Format(FText::FromString("{0}{1}"),
			InDisplayName,
			(FText::Format(FText::FromString(" + {0}%"), FText::AsNumber(InValue * 100)))
			);
		UpgradeText->SetText(DisplayFormat);
		break;
	case EStatModifierType::PercentMinus:
		DisplayFormat = FText::Format(FText::FromString("{0}{1}"),
			InDisplayName,
			(FText::Format(FText::FromString(" - {0}%"), FText::AsNumber(InValue * 100)))
			);
		UpgradeText->SetText(DisplayFormat);
		break;
	case EStatModifierType::PercentMultiply:
		DisplayFormat = FText::Format(FText::FromString("{0}{1}"),
			InDisplayName,
			(FText::Format(FText::FromString(" x {0}%"), FText::AsNumber(InValue * 100)))
			);
		UpgradeText->SetText(DisplayFormat);
		break;
	case EStatModifierType::PercentDivide:
		DisplayFormat = FText::Format(FText::FromString("{0}{1}"),
				InDisplayName,
				(FText::Format(FText::FromString(" / {0}%"), FText::AsNumber(InValue * 100)))
				);
		UpgradeText->SetText(DisplayFormat);
		break;
	case EStatModifierType::None:
		DisplayFormat = FText::Format(FText::FromString("{0}{1}"),
			InDisplayName,
			(FText::Format(FText::FromString(" + {0} \n {1}"),
				FText::AsNumber(InValue),
				FText::FromString(TEXT("업그레이드 구현 필요"))))
			);
		UpgradeText->SetText(DisplayFormat);
		break;
	default:
		break;
	}
}

void USanzoUpgradeButtonWidget::HandleButtonClicked()
{
	OnUpgradeButtonClicked.Broadcast(CachedOption);
}

UTexture2D* USanzoUpgradeButtonWidget::GetTextureByRarity(EUpgradeRarity Rarity)
{
	if (RarityTextures.Contains(Rarity))
	{
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

