// stage 이동 전 정보 업데이트(save)
#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SanzoGameInstance.generated.h"

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

	// 스탯 정보


	// 스탯 업데이트 함수


	// 외형 정보


  // 외형 업데이트 함수

#pragma endregion 최윤서

};
