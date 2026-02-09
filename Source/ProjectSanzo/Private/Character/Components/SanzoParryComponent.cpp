// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/SanzoParryComponent.h"

USanzoParryComponent::USanzoParryComponent()
{

  PrimaryComponentTick.bCanEverTick = false;

  // ...
}


void USanzoParryComponent::BeginPlay()
{
  Super::BeginPlay();

  // ...

}


void USanzoParryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  // ...
}

