// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SanzoStatInfoTextWidget.h"

#include "Components/TextBlock.h"
#include "DSP/AudioDebuggingUtilities.h"

void USanzoStatInfoTextWidget::SetStatInfoText(FText InName, EStatModifierType InModifierType, float InCurrentValue,float InBaseValue, float InBonusValue)
{
	if (StatName)
	{
		StatName->SetText(InName);
	}
	
	if (CurrentValue)
	{
		CurrentValue->SetText(FText::AsNumber(InCurrentValue));
	}
	
	if (BaseValue)
	{
		BaseValue->SetText(FText::AsNumber(InBaseValue));
	}
	
	if (UpgradeValue)
	{
		SetBonusValueText(InModifierType, InBonusValue);
	}
		
}

void USanzoStatInfoTextWidget::SetBonusValueText(EStatModifierType InModifierType, float InBonusValue)
{
	if (InBonusValue > 0.0f)
	{
		FText BonusFormat;
		switch (InModifierType)
		{
		case EStatModifierType::FlatPlus:
			BonusFormat = FText::Format(FText::FromString(TEXT("+{0}")), FText::AsNumber(InBonusValue));
			UpgradeValue->SetText(BonusFormat);
			UpgradeValue->SetVisibility(ESlateVisibility::Visible);
			break;
		case EStatModifierType::FlatMinus:
			BonusFormat = FText::Format(FText::FromString(TEXT("-{0}")), FText::AsNumber(InBonusValue));
			UpgradeValue->SetText(BonusFormat);
			UpgradeValue->SetVisibility(ESlateVisibility::Visible);
			break;
		case EStatModifierType::PercentMultiplyPlus:
			BonusFormat = FText::Format(FText::FromString(TEXT("x{0}")), FText::AsNumber(1+InBonusValue));
			UpgradeValue->SetText(BonusFormat);
			UpgradeValue->SetVisibility(ESlateVisibility::Visible);
			break;
		case EStatModifierType::PercentMultiplyMinus:
			BonusFormat = FText::Format(FText::FromString(TEXT("x{0}")), FText::AsNumber(1-InBonusValue));
			UpgradeValue->SetText(BonusFormat);
			UpgradeValue->SetVisibility(ESlateVisibility::Visible);
			break;
		default:
			break;
		}
	}
	else
	{
		// 보너스가 없으면 숨기거나 빈칸
		UpgradeValue->SetVisibility(ESlateVisibility::Collapsed);
	}
}
