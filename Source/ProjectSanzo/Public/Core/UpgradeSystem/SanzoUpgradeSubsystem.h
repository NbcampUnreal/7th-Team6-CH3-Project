// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/UpgradeSystem/UpgradeDataRow.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SanzoUpgradeSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnUpgradeSelected, const FUpgradeOption&);

UCLASS()
class PROJECTSANZO_API USanzoUpgradeSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
#pragma region UpgradeSystem
	
public:
	USanzoUpgradeSubsystem();
	//위젯에서 표시할 랜덤 선택지 생성 함수
	UFUNCTION(BlueprintCallable, Category = "Upgrade System")
	TArray<FUpgradeOption> GeneratedRandomOptions();
	
	//플레이어가 선택한 업그레이드 확정 및 저장
	UFUNCTION(BlueprintCallable, Category = "Upgrade System")
	void ProcessSelectedUpgrade(const FUpgradeOption& Selected);
	
	UFUNCTION(BlueprintCallable, Category = "Upgrade System")
	const TMap<FUpgradeStatKey, float>& GetUpgradeTotalMap() const { return UpgradeTotalMap; }
	
	UFUNCTION(BlueprintCallable, Category = "Upgrade System")
	const TMap<FName, int32>& GetSelectedTotalMap() const { return SelectedTotalMap; }

	FOnUpgradeSelected OnUpgradeSelected;
	
protected:
	UPROPERTY(BlueprintReadWrite, Category = "Upgrade System|DataTable")
	UDataTable* UpgradeDataTable;
	
private:
	EUpgradeRarity RollRarity();
	
	FUpgradeOption ConvertToOption(const FUpgradeDataRow* SelectedRow);
	
	//빈 값 생성용 함수
	FUpgradeOption GetNoneOption();
	
	//선택된 업그레이드 Limit 값 계산
	UPROPERTY()
	TMap<FUpgradeStatKey, float> UpgradeTotalMap;
	
	//선택된 업그레이드 카운트 저장용 Map
	UPROPERTY()
	TMap<FName, int32> SelectedTotalMap;
	
	//업그레이드 한도 도달 ID 저장
	UPROPERTY()
	TSet<FName> LimitedUpgradeIDs;

#pragma endregion 이준로
#pragma region ResetUpgradeData
	
public:
	void ResetUpgradeData();
	
#pragma endregion 이준로
};
