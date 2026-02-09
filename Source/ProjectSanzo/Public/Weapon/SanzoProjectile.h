// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SanzoProjectile.generated.h"

UCLASS()
class PROJECTSANZO_API ASanzoProjectile : public AActor
{
  GENERATED_BODY()

public:
  ASanzoProjectile();

protected:
  virtual void BeginPlay() override;

public:
  virtual void Tick(float DeltaTime) override;

};
