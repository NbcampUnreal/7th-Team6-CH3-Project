// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/UpgradeSystem/SanzoUpgradeSubsystem.h"

USanzoUpgradeSubsystem::USanzoUpgradeSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTableAsset(TEXT("/Game/DataTables/DT_UpgradeData.DT_UpgradeData"));
	
	if (DataTableAsset.Succeeded())
	{
		UpgradeDataTable = DataTableAsset.Object;
	}
}

TArray<FUpgradeOption> USanzoUpgradeSubsystem::GeneratedRandomOptions()
{
	TArray<FUpgradeOption> result;
	if (!UpgradeDataTable)
	{
		return result;
	}
	
	TArray<FUpgradeDataRow*> AllRows;
	UpgradeDataTable->GetAllRows<FUpgradeDataRow>(TEXT("GeneratedRandomOption"), AllRows);
	
	TArray<FUpgradeDataRow*> Candidates;
	for (auto Row : AllRows)
	{
		if (!LimitedUpgradeIDs.Contains(Row->UpgradeID))
		{
			Candidates.Add(Row);
		} 
	}
	
	int32 NumbersToPick = FMath::Min(3, Candidates.Num());
	for (int32 i = 0; i < NumbersToPick; i++)
	{
		int32 RandomIndex = FMath::RandRange(i, Candidates.Num()-1);
		Candidates.Swap(i, RandomIndex);
		
		FUpgradeDataRow* SelectedRow = Candidates[i];
		
		FUpgradeOption NewOption;
		NewOption.UpgradeID = SelectedRow->UpgradeID;
		NewOption.DisplayName = SelectedRow->DisplayName;
		NewOption.UpgradeTarget = SelectedRow->UpgradeTarget;
		NewOption.UpgradeType = SelectedRow->UpgradeType;
		NewOption.Value = SelectedRow->Value;
		NewOption.Rarity = SelectedRow->Rarity;
		
		result.Add(NewOption);
	}
	return result;
}

void USanzoUpgradeSubsystem::ProcessSelectedUpgrade(const FUpgradeOption& Selected)
{
	UpgradeHistory.Add(Selected);
	
	// UpgradeComponent에 Data 전달하는 로직 추가
}

