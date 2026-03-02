// stage 이동 전 정보 업데이트(save)
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/GameInstance.h"
#include "Character/Components/SanzoStatComponent.h"
#include "SanzoGameInstance.generated.h"

class ASanzoCharacter;
UCLASS()
class PROJECTSANZO_API USanzoGameInstance : public UGameInstance
{
  GENERATED_BODY()

public:
  USanzoGameInstance();

#pragma region Save & Load
	// 스테이지 정보
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stage")
	TArray<TSoftObjectPtr<UWorld>> StageLevels;

	UPROPERTY(BlueprintReadOnly, Category = "Stage")
	int32 CurrentStageIndex = 0;

  // 스테이지 이동 함수
  UFUNCTION(BlueprintCallable, Category = "Stage")
	void MoveToNextStage();
  void Restart();

	// 스탯 정보
	UPROPERTY()
	FSanzoSaveStatData CachedStatData;
	UPROPERTY()
	int32 CachedAmmo;

	// 총 관련 정보
	UPROPERTY()
	float CachedGunDamage = -1.0f;
	UPROPERTY()
	float CachedGunFireRate = -1.0f;
	UPROPERTY()
	float CachedHomingMissileChance = -1.0f;

	// 활 관련 정보
	UPROPERTY()
	float CachedBowDamage = -1.0f;
	UPROPERTY()
	float CachedBowChargeTime = -1.0f;
	UPROPERTY()
	bool bCachedBowMultiShot = false;
	// 스탯 백업
	void BackupStat(ASanzoCharacter* Player);
  // 스탯 복원
	void RestoreStat(ASanzoCharacter* Player);
	void InitSetup();

#pragma endregion 최윤서
	
#pragma region UIMediaData
	
	UPROPERTY(BlueprintReadWrite, Category="UIMediaData")
	FGameplayTag MediaPlayTag = FGameplayTag::RequestGameplayTag(FName("UI.State.Opening"));
	
#pragma endregion 이준로
};
