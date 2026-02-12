// 실시간 상태 동기화

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/GameStateBase.h"
#include "SanzoGameState.generated.h"

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
  
  // 전투 정보
  // 섬멸전일때는 적 수, 버티기일때는 생존 시간
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
  float CurrentCount;
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
  float TotalCount;

#pragma endregion 최윤서

#pragma region UI
	
	FTimerHandle HUDUpdateTimerHandle;
	void UpdateHUD();

	//임시 함수
	//void StartStage();


#pragma endregion 이준로
};
