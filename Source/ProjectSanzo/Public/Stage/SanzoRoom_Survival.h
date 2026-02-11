// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Stage/SanzoRoomBase.h"
#include "SanzoRoom_Survival.generated.h"

UCLASS()
class PROJECTSANZO_API ASanzoRoom_Survival : public ASanzoRoomBase
{
  GENERATED_BODY()

public:
  virtual void BeginRoomSequence() override;
  virtual void EndRoomSequence() override;
};
