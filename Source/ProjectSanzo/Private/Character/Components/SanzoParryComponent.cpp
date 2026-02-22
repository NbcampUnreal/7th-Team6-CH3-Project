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
        //특정 몽타주가 끝날 때 실행할 델리게이트 설정
        
        AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, ParryMontage);
      }
    }
  }
}


void USanzoParryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  // ...
}

