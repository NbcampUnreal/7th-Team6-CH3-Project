// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/SanzoParryComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
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

void USanzoParryComponent::SuccessParry()
{
  if (ParrySuccessMontage)
  {
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (OwnerCharacter)
    {
      UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
      FName CurrentSection = AnimInstance->Montage_GetCurrentSection();
      
      if (AnimInstance && CurrentSection != FName("Success")) // 성공 모션이 안나오고 있을 때만 성공 모션으로 전환
      {
        AnimInstance->Montage_JumpToSection(FName("Success"), ParryMontage);
      }

      if (ParrySound)
      {
        UGameplayStatics::PlaySound2D(GetWorld(), ParrySound); //성공시 소리 재생
      }

      GetWorld()->GetWorldSettings()->SetTimeDilation(0.25f); //성공시 시간 느리게
      GetWorld()->GetTimerManager().SetTimer(
        SlowTimerHandle,
        [this]()
        {
          GetWorld()->GetWorldSettings()->SetTimeDilation(1.0f); //시간 정상화
        }, 
        0.1f, //0.1초 후 시간 원래대로
        false);
    }
  }
}


void USanzoParryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  // ...
}

