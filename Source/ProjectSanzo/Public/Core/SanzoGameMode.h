#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Core/SanzoStageTypes.h"
#include "SanzoGameMode.generated.h"

UCLASS()
class PROJECTSANZO_API ASanzoGameMode : public AGameModeBase
{
  GENERATED_BODY()

public:
  ASanzoGameMode();

  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stage")
  ESanzoStageType CurrentStageType;

  void InitStageType();

  // 스테이지 흐름
  void StartStage();
  void OnStageCleared();

  // 성장
  void TriggerUpgradeSelection();
  void OnUpgradeSelected();
};
