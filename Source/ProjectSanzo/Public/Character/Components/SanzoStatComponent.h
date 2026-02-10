// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SanzoStatComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTSANZO_API USanzoStatComponent : public UActorComponent
{
  GENERATED_BODY()

public:
  USanzoStatComponent();

protected:
  virtual void BeginPlay() override;

  //stamina 관련 변수
  float CurrentStamina;
  float MaxStamina;
  float StaminaRegenRate;
  float StaminaConsumptionRate;
  float StaminaThresholdForActions;
  float StaminaRegenDelay;
  float TimeSinceLastStaminaUse;
  float DodgeStaminaCost;
  float SprintStaminaCostPerSecond;

  //float Health;
  //float MaxHealth;
  //float HealthRegenRate;
  //float TimeSinceLastHealthLoss;
  //float HealthRegenDelay;
  //float PoisonDamagePerSecond;
  //float TimeSinceLastPoisonDamage;
  //float PoisonDuration;
  //float TimeSincePoisoned;
  //float Defense;
  //float AttackPower;
  //float CriticalHitChance;

public:
  virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

  void ConsumeStamina(float Amount);
  float GetStamina() const { return CurrentStamina; }

};
