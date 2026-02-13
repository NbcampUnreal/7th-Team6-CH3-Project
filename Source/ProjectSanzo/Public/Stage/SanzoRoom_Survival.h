// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Stage/SanzoRoomBase.h"
#include "SanzoRoom_Survival.generated.h"

UCLASS()
class PROJECTSANZO_API ASanzoRoom_Survival : public ASanzoRoomBase
{
  GENERATED_BODY()
#pragma region Survival Room
public:
  ASanzoRoom_Survival();
  virtual void BeginRoomSequence() override;
  virtual void EndRoomSequence() override;

  // 시간
  float CurrentTime;
  float TotalTime;
  void UpdateTime();
#pragma endregion 최윤서
};
