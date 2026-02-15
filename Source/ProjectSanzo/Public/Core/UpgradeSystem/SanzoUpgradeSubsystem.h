// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/UpgradeSystem/UpgradeDataRow.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SanzoUpgradeSubsystem.generated.h"

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
	const TArray<FUpgradeOption>& GetUpgradeHistory() const { return UpgradeHistory;}

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Upgrade System|DataTable")
	UDataTable* UpgradeDataTable;
	
private:
	//선택된 업그레이드 기록용
	UPROPERTY()
	TArray<FUpgradeOption> UpgradeHistory;
	
	UPROPERTY()
	TSet<FName> LimitedUpgradeIDs;

#pragma endregion 이준로
};
