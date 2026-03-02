// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SanzoStatInfoWidget.h"

#include "Components/ScrollBox.h"
#include "UI/SanzoStatInfoTextWidget.h"

void USanzoStatInfoWidget::SetStatInfo(TArray<FStatusDisplayData> CurrentStatData,
                                       const TMap<FUpgradeStatKey, float>& UpgradeTotalMap)
{
	if (StatScrollBox)
	{
		StatScrollBox->ClearChildren();
	}
	if (GunScrollBox)
	{
		GunScrollBox->ClearChildren();
	}
	if (BowScrollBox)
	{
		BowScrollBox->ClearChildren();
	}
	
	for (const FStatusDisplayData& StatData : CurrentStatData)
	{
		//보너스 값 찾기
		float BonusValue = 0;
		
		FUpgradeStatKey SearchKey(StatData.UpgradeType);
		if (UpgradeTotalMap.Contains(SearchKey))
		{
			BonusValue = UpgradeTotalMap[SearchKey];
		}
		
		//들어갈 박스 찾기
		UScrollBox* TargetBox = nullptr;
		
		switch (StatData.DisplayTarget)
		{
		case EUpgradeTarget::Character:
			TargetBox = StatScrollBox;
			break;
		case EUpgradeTarget::Gun:
			TargetBox = GunScrollBox;
			break;
		case EUpgradeTarget::Bow:
			TargetBox = BowScrollBox;
			break;
		default:
			break;
		}
		
		//안에 들어갈 Text 설정
		if (TargetBox && StatInfoTextWidgetClass)
		{
			USanzoStatInfoTextWidget* NewStatInfo = CreateWidget<USanzoStatInfoTextWidget>(this, StatInfoTextWidgetClass);
			if (NewStatInfo)
			{
				NewStatInfo->SetStatInfoText(StatData.DisplayText, StatData.ModifierType, StatData.Value, StatData.BaseValue, BonusValue);
				//스크롤 박스에 추가
				TargetBox->AddChild(NewStatInfo);
			}
		}
	}
}
