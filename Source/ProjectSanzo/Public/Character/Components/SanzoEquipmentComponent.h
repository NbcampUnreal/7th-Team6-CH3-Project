// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SanzoEquipmentComponent.generated.h"

class ASanzoCharacter;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTSANZO_API USanzoEquipmentComponent : public UActorComponent
{
  GENERATED_BODY()

public:
  USanzoEquipmentComponent();

protected:
  virtual void BeginPlay() override;
  ASanzoCharacter* GetOwnerCharacter();


public:
  virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
  

  UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Equipment|Class")
  TSubclassOf<class ASanzoWeaponBase> WeaponClass;
  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Equipment|Class")
  ASanzoWeaponBase* CurrentWeapon;
  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Equipment|Class")
  ASanzoCharacter* OwnerCharacter;

};
