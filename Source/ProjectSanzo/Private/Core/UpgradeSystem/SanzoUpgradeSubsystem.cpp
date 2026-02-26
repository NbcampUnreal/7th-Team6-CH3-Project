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
	TArray<FUpgradeStatKey> CurrentTurnPickedTypes;
	
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
			FUpgradeStatKey RowKey(Row->UpgradeTarget, Row->UpgradeType);
			// Rarity가 맞지 않거나 이미 선택된 업그레이드 선택지 일 경우 스킵
			if (Row->Rarity != TargetRarity || CurrentTurnPickedTypes.Contains(RowKey))
			{
				continue;
			}
			//해당 옵션의 Key로 LimitValue 계산
			float CurrentTotalValue = UpgradeTotalMap.Contains(RowKey) ? UpgradeTotalMap[RowKey] : 0.0f;
			if (Row->LimitValue > 0 && CurrentTotalValue >= Row->LimitValue)
			{
				continue;
			}
			
			Candidates.Add(Row);
			TotalWeight += Row->SpawnWeight;
		}
		//만약 해당 등급의 후보가 없으면 다시 RollRarity부터 시작
		if (Candidates.Num() == 0) continue;
		
		//가중치 기반 랜덤 선택
		int32 RandomWeight = FMath::RandRange(1, TotalWeight);
		int32 WeightSum = 0;
		for (auto Candidate : Candidates)
		{
			WeightSum += Candidate->SpawnWeight;
			if (RandomWeight <= WeightSum)
			{
				CurrentTurnPickedTypes.Add(FUpgradeStatKey(Candidate->UpgradeTarget, Candidate->UpgradeType));
				Result.Add(ConvertToOption(Candidate));
				break;
			}
		}
	}
	// 옵션이 3개 채워지지 않았을 경우 빈 값 입력
	while (Result.Num() < 3)
	{
		Result.Add(GetNoneOption());
	}
	
	return Result;
}

void USanzoUpgradeSubsystem::ProcessSelectedUpgrade(const FUpgradeOption& Selected)
{
	if (Selected.UpgradeID == TEXT("NONE") || Selected.UpgradeID.IsNone()) return;

	FUpgradeStatKey Key(Selected.UpgradeTarget, Selected.UpgradeType);
	UpgradeTotalMap.FindOrAdd(Key) += Selected.Value;

	SelectedTotalMap.FindOrAdd(Selected.UpgradeID) += 1;
	
	// UpgradeComponent에 Data 전달하는 로직 
	OnUpgradeSelected.Broadcast(Selected);
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

FUpgradeOption USanzoUpgradeSubsystem::GetNoneOption()
{
	FUpgradeOption EmptyOption;
		
	EmptyOption.UpgradeID = TEXT("NONE"); // ID를 통해 UI와 로직에서 구분
	EmptyOption.DisplayName = FText::FromString(TEXT("더 이상 강화 가능한 항목이 없습니다."));
	EmptyOption.UpgradeTarget = EUpgradeTarget::Character;
	EmptyOption.UpgradeType = EUpgradeType::MaxHealth;
	EmptyOption.Value = 0.0f;
	EmptyOption.Rarity = EUpgradeRarity::Common;
	EmptyOption.IconTexture = nullptr;
	
	return EmptyOption;
}

void USanzoUpgradeSubsystem::ResetUpgradeData()
{
	UpgradeTotalMap.Empty();
	SelectedTotalMap.Empty();
	LimitedUpgradeIDs.Empty();
	
	UE_LOG(LogLJR, Warning, TEXT("UpgradeSubsystem: 업그레이드 데이터 초기화 완료"))
}

