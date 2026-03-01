// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SanzoUpgradeInfo.h"

#include "Common/SanzoLog.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void USanzoUpgradeInfo::SetUpgradeInfoBlock(FName UpgradeID, int32 Count)
{
	if (!UpgradeDataTable) return;
	
	static const FString ContextString(TEXT("UpgradeLookUP"));
	FUpgradeDataRow* FoundRow = UpgradeDataTable->FindRow<FUpgradeDataRow>(UpgradeID, ContextString);
	
	if (FoundRow)
	{
		TSoftObjectPtr<UTexture2D> IconTexture = FoundRow->IconTexture;
		EUpgradeRarity Rarity = FoundRow->Rarity;
		
		SetIconBackground(Rarity);
		IconImage->SetBrushFromSoftTexture(IconTexture);
	}
	if (UpgradeCount)
	{
		UpgradeCount->SetText(FText::Format(FText::FromString("x {0}"), FText::AsNumber(Count)));
	}
	
}

void USanzoUpgradeInfo::SetIconBackground(EUpgradeRarity UpgradeRarity)
{
	switch (UpgradeRarity)
	{
	case EUpgradeRarity::Common:
		if (IconBackgrounds[0])
		{
			UE_LOG(LogLJR, Warning, TEXT("노말 업그레이드 선택됨"));
			IconBackground->SetBrushFromTexture(IconBackgrounds[0]);
		}
		break;
	case EUpgradeRarity::Rare:
		if (IconBackgrounds[1])
		{
			IconBackground->SetBrushFromTexture(IconBackgrounds[1]);
		}
		break;
	case EUpgradeRarity::Epic:
		if (IconBackgrounds[2])
		{
			IconBackground->SetBrushFromTexture(IconBackgrounds[2]);
		}
		break;
	case EUpgradeRarity::Legend:
		if (IconBackgrounds[3])
		{
			IconBackground->SetBrushFromTexture(IconBackgrounds[3]);
		}
	case EUpgradeRarity::Link:
		if (IconBackgrounds[4])
		{
			IconBackground->SetBrushFromTexture(IconBackgrounds[4]);
		}
		break;
	default:
		break;
	}
}
