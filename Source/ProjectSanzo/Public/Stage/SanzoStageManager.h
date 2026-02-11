// 스테이지 전체 관리 (방 순서, 다음 방 개방 지시)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/SanzoStageTypes.h"
#include "Stage/SanzoRoomBase.h"
#include "Stage/SanzoStagegate.h"
#include "SanzoStageManager.generated.h"

class ASanzoGameMode;
UCLASS()
class PROJECTSANZO_API ASanzoStageManager : public AActor
{
  GENERATED_BODY()

public:
  ASanzoStageManager();

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stage")
  ESanzoStageType StageType;

  // Room
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room")
  TSubclassOf<ASanzoRoomBase> ExterminationRoomClass;
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room")
  TSubclassOf<ASanzoRoomBase> SurvivalRoomClass;
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room")
  TSubclassOf<ASanzoRoomBase> BossRoomClass;
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room")
  ASanzoRoomBase* CurrentRoom;
  UPROPERTY()
  ASanzoGameMode* SGM;

protected:
  virtual void BeginPlay() override;

public:
  virtual void Tick(float DeltaTime) override;
  UFUNCTION()
  void StartStage();
  UFUNCTION()
  void OnRoomCleared();
  void MoveToNextRoom();

  // State
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State")
  FGameplayTag CurrentState;

  UPROPERTY(EditDefaultsOnly, Category = "State")
  FGameplayTag StageActivedTag;
  UPROPERTY(EditDefaultsOnly, Category = "State")
  FGameplayTag StageClearedTag;
  UPROPERTY(EditDefaultsOnly, Category = "State")
  FGameplayTag MoveNextTag;

  UPROPERTY(EditAnywhere, Category = "Stage")
  ASanzoStageGate* StageGate;

  UFUNCTION()
  void SetState(FGameplayTag NewState);
};
