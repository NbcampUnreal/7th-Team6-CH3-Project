// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SanzoEquipmentComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTSANZO_API USanzoEquipmentComponent : public UActorComponent
{
  GENERATED_BODY()

public:
  USanzoEquipmentComponent();

protected:
  virtual void BeginPlay() override;



public:
  virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
  TSubclassOf<class ASanzoWeaponBase> WeaponClass;
  ASanzoWeaponBase* WeaponInstance;
};
