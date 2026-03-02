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
	if (IconBackgrounds.Find(UpgradeRarity))
	{
		if (IconBackground)
		{
			IconBackground->SetBrushFromTexture(IconBackgrounds[UpgradeRarity]);
		}
	}
}
