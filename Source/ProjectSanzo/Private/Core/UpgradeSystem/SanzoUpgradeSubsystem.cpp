// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/UpgradeSystem/SanzoUpgradeSubsystem.h"

#include "Common/SanzoLog.h"

USanzoUpgradeSubsystem::USanzoUpgradeSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTableAsset(TEXT("/Game/DataTables/DT_UpgradeDataTable.DT_UpgradeDataTable"));
	
	if (DataTableAsset.Succeeded())
	{
		UpgradeDataTable = DataTableAsset.Object;
	}
}

TArray<FUpgradeOption> USanzoUpgradeSubsystem::GeneratedRandomOptions()
{
	TArray<FUpgradeOption> Result;
	if (!UpgradeDataTable)
	{
		return Result;
	}

	TArray<FUpgradeDataRow*> AllRows;
	UpgradeDataTable->GetAllRows<FUpgradeDataRow>(TEXT("GeneratedRandomOption"), AllRows);

	//중복 옵션 방지용 Array
	TArray<FName> CurrentTurnPickedIDs;
	//무한루프 방지용 최대 시도 횟수
	int32 MaxAttempts = 50;

	while (Result.Num() < 3 && MaxAttempts > 0)
	{
		MaxAttempts--;
		//선택될 옵션의 Rarity 뽑기
		EUpgradeRarity TargetRarity = RollRarity();

		//저장될 후보군 Array
		TArray<FUpgradeDataRow*> Candidates;
		//가중치 계산용 전체 가중치
		int32 TotalWeight = 0;

		for (auto Row : AllRows)
		{
			// Rarity가 맞지 않거나 이미 선택된 업그레이드 선택지 일 경우 스킵
			if (Row->Rarity != TargetRarity || CurrentTurnPickedIDs.Contains(Row->UpgradeID))
			{
				continue;
			}
			FUpgradeStatKey Key(Row->UpgradeTarget, Row->UpgradeType);
			float CurrentTotalValue = UpgradeTotalMap.Contains(Key) ? UpgradeTotalMap[Key] : 0.0f;

			if (Row->LimitValue > 0 && CurrentTotalValue >= Row->LimitValue)
			{
				continue;
			}
			Candidates.Add(Row);
			TotalWeight += Row->SpawnWeight;
		}
		if (Candidates.Num() > 0)
		{
			int32 RandomWeight = FMath::RandRange(1, TotalWeight);
			int32 WeightSum = 0;

			for (auto Candidate : Candidates)
			{
				WeightSum += Candidate->SpawnWeight;
				if (RandomWeight <= WeightSum)
				{
					CurrentTurnPickedIDs.Add(Candidate->UpgradeID);
					Result.Add(ConvertToOption(Candidate));
					break;
				}
			}
		}
	}
	// 옵션이 3개 채워지지 않았을 경우 빈 값 입력
	while (Result.Num() < 3)
	{
		FUpgradeOption EmptyOption;
		
		EmptyOption.UpgradeID = TEXT("NONE"); // ID를 통해 UI와 로직에서 구분
		EmptyOption.DisplayName = FText::FromString(TEXT("더 이상 강화 가능한 항목이 없습니다."));
		EmptyOption.UpgradeTarget = EUpgradeTarget::Character;
		EmptyOption.UpgradeType = EUpgradeType::MaxHealth;
		EmptyOption.Value = 0.0f;
		EmptyOption.Rarity = EUpgradeRarity::Common;
		EmptyOption.IconTexture = nullptr;
		
		Result.Add(EmptyOption);
	}
	
	return Result;
}

void USanzoUpgradeSubsystem::ProcessSelectedUpgrade(const FUpgradeOption& Selected)
{
	if (Selected.UpgradeID != TEXT("NONE") && !Selected.UpgradeID.IsNone())
	{
		FUpgradeStatKey Key(Selected.UpgradeTarget, Selected.UpgradeType);
		UpgradeTotalMap.FindOrAdd(Key) += Selected.Value;
	
		UpgradeHistory.Add(Selected);
	
		// UpgradeComponent에 Data 전달하는 로직 추가
	}
	else
	{
		UE_LOG(LogLJR, Log, TEXT("빈 업그레이드 선택됨"));
	}
}

EUpgradeRarity USanzoUpgradeSubsystem::RollRarity()
{
	int32 RandomValue = FMath::RandRange(1, 100);

	if (RandomValue <= 10) return EUpgradeRarity::Legend;
	if (RandomValue <= 30) return EUpgradeRarity::Epic;
	if (RandomValue <= 60) return EUpgradeRarity::Rare;

	return EUpgradeRarity::Common;
}

FUpgradeOption USanzoUpgradeSubsystem::ConvertToOption(const FUpgradeDataRow* SelectedRow)
{
	FUpgradeOption NewOption;
	NewOption.UpgradeID = SelectedRow->UpgradeID;
	NewOption.DisplayName = SelectedRow->DisplayName;
	NewOption.UpgradeTarget = SelectedRow->UpgradeTarget;
	NewOption.UpgradeType = SelectedRow->UpgradeType;
	NewOption.Value = SelectedRow->Value;
	NewOption.Rarity = SelectedRow->Rarity;
	NewOption.IconTexture = SelectedRow->IconTexture;
	return NewOption;
}

