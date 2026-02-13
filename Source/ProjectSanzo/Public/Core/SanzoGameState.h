// 실시간 상태 동기화

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/GameStateBase.h"
#include "SanzoGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStageProgressChangedDelegate, float, StageProgressPercent);

UCLASS()
class PROJECTSANZO_API ASanzoGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ASanzoGameState();

	virtual void BeginPlay() override;

#pragma region synchronization
  // 게임 상태
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State")
  FGameplayTagContainer GameStateTags;

	// 스테이지 정보
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stage")
	int32 CurrentStageType;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stage")
	int32 CurrentStageIndex;

	// 실시간 전투 정보
	// 섬멸전일때는 적 수, 버티기일때는 생존 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float CurrentCount;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float TotalCount;

	// 전투 결과 정보
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
  int32 TotalKillCount;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
  float TotalSurvivalTime;

  // 전투 정보 업데이트
  void UpdateStageInfo(float Current, float Total);
  void UpdateStageResult(int32 KillCount, float SurvivalTime);
#pragma endregion 최윤서

#pragma region UI

	FOnStageProgressChangedDelegate OnStageProgressChanged;

	void UpdateStageProgressBar();

	void OpenHUD();

	FTimerHandle UpdateStageProgressBarTimer;
#pragma endregion 이준로
};
