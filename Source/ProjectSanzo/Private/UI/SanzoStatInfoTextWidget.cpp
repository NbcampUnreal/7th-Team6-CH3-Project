// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SanzoStatInfoTextWidget.h"

#include "Components/TextBlock.h"

void USanzoStatInfoTextWidget::SetStatInfoText(FText InName, float InCurrentValue,float InBaseValue, float InBonusValue)
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
		if (InBonusValue > 0.0f)
		{
			FText BonusFormat = FText::Format(FText::FromString(TEXT("+{0}")), FText::AsNumber(InBonusValue));
			UpgradeValue->SetText(BonusFormat);
			UpgradeValue->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			// 보너스가 없으면 숨기거나 빈칸
			UpgradeValue->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
		
}
