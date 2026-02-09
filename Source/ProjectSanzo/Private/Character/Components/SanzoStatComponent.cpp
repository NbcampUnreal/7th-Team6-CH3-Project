// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/SanzoStatComponent.h"

USanzoStatComponent::USanzoStatComponent()
{
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

