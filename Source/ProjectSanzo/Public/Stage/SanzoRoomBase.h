// 방 공통 로직

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "SanzoRoomBase.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnRoomCleared);

UCLASS()
class PROJECTSANZO_API ASanzoRoomBase : public AActor
{
  GENERATED_BODY()

public:
  ASanzoRoomBase();

  UPROPERTY()
  FGameplayTagContainer RoomStateTags;

  FOnRoomCleared OnRoomCleared;

  virtual void BeginPlay() override;

  virtual void BeginRoomSequence();
  virtual void EndRoomSequence();

public:
  virtual void Tick(float DeltaTime) override;

};
