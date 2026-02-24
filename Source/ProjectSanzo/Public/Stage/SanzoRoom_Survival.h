// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Stage/SanzoRoomBase.h"
#include "SanzoRoom_Survival.generated.h"

class ASanzoSpawnGate;
UCLASS()
class PROJECTSANZO_API ASanzoRoom_Survival : public ASanzoRoomBase
{
  GENERATED_BODY()
#pragma region Survival Room
public:
  virtual void BeginPlay() override;

  ASanzoRoom_Survival();
  virtual void BeginRoomSequence() override;
  virtual void EndRoomSequence() override;

  // 시간
  float TotalTime;
  virtual void UpdateTime() override;
  float ElapsedTime = 10.f; // 사이클 카운트
  float CycleTime = 15.f; // 스폰 사이클
#pragma endregion 최윤서
};
