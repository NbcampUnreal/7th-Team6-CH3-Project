// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SanzoUpgradeButtonWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Core/UpgradeSystem/SanzoUpgradeSubsystem.h"

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
	
	if (UpgradeButton)
	{
		UpgradeButton->SetBackgroundColor(GetColorByRarity(InputOption.Rarity));
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
	
}

void USanzoUpgradeButtonWidget::HandleButtonClicked()
{
	OnUpgradeButtonClicked.Broadcast(CachedOption);
}

FLinearColor USanzoUpgradeButtonWidget::GetColorByRarity(EUpgradeRarity Rarity)
{
	switch (Rarity)
	{
	case EUpgradeRarity::Legend:
		return FLinearColor(1.0f, 0.72f, 0.0f);

	case EUpgradeRarity::Epic:
		return FLinearColor(0.6f, 0.2f, 0.9f);

	case EUpgradeRarity::Rare:
		return FLinearColor(0.0f, 0.4f, 0.9f);

	case EUpgradeRarity::Common:
		return FLinearColor(0.5f,0.5f,0.5f);
		
	default:
		return FLinearColor::Black;
	}
}