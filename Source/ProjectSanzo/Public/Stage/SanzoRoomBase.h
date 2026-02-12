// 방 공통 로직

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Stage/SanzoStageGate.h"
#include "SanzoRoomBase.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnRoomCleared);

UCLASS()
class PROJECTSANZO_API ASanzoRoomBase : public AActor
{
  GENERATED_BODY()
#pragma region 전투 Base
public:
  ASanzoRoomBase();

  FOnRoomCleared OnRoomCleared;

  virtual void BeginPlay() override;

  virtual void BeginRoomSequence();
  virtual void EndRoomSequence();

public:
  virtual void Tick(float DeltaTime) override;

  FTimerHandle RoomSequenceTimerHandle;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stage")
  ASanzoStageGate* StageGate;
#pragma endregion 최윤서
};
