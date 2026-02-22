// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/SanzoParryComponent.h"
#include "GameFramework/Character.h"
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

void USanzoParryComponent::PlayParryMontage()
{
  
  if (ParryMontage)
  {
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (OwnerCharacter)
    {
      UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
      if (AnimInstance)
      {
        AnimInstance->Montage_Play(ParryMontage);
      }
    }
  }
}


void USanzoParryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  // ...
}

