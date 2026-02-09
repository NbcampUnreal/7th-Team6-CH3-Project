// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SanzoItemBase.generated.h"

UCLASS()
class PROJECTSANZO_API ASanzoItemBase : public AActor
{
  GENERATED_BODY()

public:
  ASanzoItemBase();

protected:
  virtual void BeginPlay() override;

public:
  virtual void Tick(float DeltaTime) override;

};
