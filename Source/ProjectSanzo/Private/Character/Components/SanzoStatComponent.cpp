// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/SanzoStatComponent.h"

USanzoStatComponent::USanzoStatComponent()
{
  CurrentStamina = 100.f;
  MaxStamina = 100.f;
  StaminaRegenRate = 5.f;
  StaminaConsumptionRate = 10.f;
  StaminaThresholdForActions = 20.f;
  StaminaRegenDelay = 2.f;
  TimeSinceLastStaminaUse = 0.f;
  DodgeStaminaCost = 15.f;
  SprintStaminaCostPerSecond = 5.f; 

  PrimaryComponentTick.bCanEverTick = false;

  // ...
}


void USanzoStatComponent::BeginPlay()
{
  Super::BeginPlay();

  // ...

}


void USanzoStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  // ...
}

void USanzoStatComponent::ConsumeStamina(float Amount)
{
  
  CurrentStamina -= Amount;
  
  
  if(Amount <= 0.f)
  {
    return;
  } 
}

