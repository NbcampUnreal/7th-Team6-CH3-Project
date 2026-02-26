// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/SanzoParryComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "GameplayTagAssetInterface.h"
#include "Character/Interface/SanzoTagEditorInterface.h"
#include "Common/SanzoGameplayTag.h"

USanzoParryComponent::USanzoParryComponent()
{
  ParryPenaltyCount = 0; //패리 페널티 초기화
  ParryCooldownTime = 0.22f; //패리 쿨타임 0.22초로 설정
  ParryPenaltyDuration = 0.4f; //패리 페널티 지속시간 0.4초로 설정
  LastParryTime = 0.f;
  PrimaryComponentTick.bCanEverTick = false;

  // ...
}


void USanzoParryComponent::BeginPlay()
{
  Super::BeginPlay();

  // ...

}

void USanzoParryComponent::ApplyParrySpamPenalty()
{
  //인터페이스 캐스트해서 캐시
  IGameplayTagAssetInterface* TagCheckInterface = Cast<IGameplayTagAssetInterface>(GetOwner());
  ISanzoTagEditorInterface* TagEditor = Cast<ISanzoTagEditorInterface>(GetOwner());

  if (TagCheckInterface)
  {
    if (TagCheckInterface->HasMatchingGameplayTag(SanzoTags::ParryPenaltyActive))
    {
      ParryPenaltyCount++; //패리 페널티 활성화 상태면 패널티 카운트 증가
    }
  }

  //게임패널티 타이머 제거 // 나중에 패리컴포넌트로 옮길 수 있는지..
  GetWorld()->GetTimerManager().SetTimer(
    ParryPenaltyTimerHandle,
    this,
    &USanzoParryComponent::ResetParrySpamPenalty,
    ParryPenaltyDuration, //초기값 0.4초
    false
  );
}

void USanzoParryComponent::ResetParrySpamPenalty()
{
  // 패리 페널티 태그 제거
  if (ISanzoTagEditorInterface* TagEditor = Cast<ISanzoTagEditorInterface>(GetOwner()))
  {
    TagEditor->RemoveGameplayTag(SanzoTags::ParryPenaltyActive);
  }
  ParryPenaltyCount = 0;

}

bool USanzoParryComponent::TryParry()
{
  if (GetWorld())
  {
    float CurrentTime = GetWorld()->GetTimeSeconds(); //패리 시도한 시간 기록
    float TimeSinceLastParry = CurrentTime - LastParryTime;

    if (TimeSinceLastParry < ParryCooldownTime)
    {
      return false; //패리 쿨타임이 아직 남아있으면 패리 재생안함
    }
    LastParryTime = CurrentTime;
  }
  else
  {
    return false; //월드가 없으면 패리 시도 실패
  }
  return true; //패리 성공
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
        switch (ParryPenaltyCount)
        {
          case 0:
            AnimInstance->Montage_JumpToSection(FName("Default"), ParryMontage); //패리 페널티 없을 때는 Start 섹션부터 재생
            break;
          case 1:
            AnimInstance->Montage_JumpToSection(FName("Parry6"), ParryMontage); //패리 페널티 1회일 때는 Parry6 섹션재생
            break;
          case 2:
            AnimInstance->Montage_JumpToSection(FName("Parry4"), ParryMontage); //패리 페널티 2회일 때는 Parry4 섹션재생
            break;
          case 3:
            AnimInstance->Montage_JumpToSection(FName("Parry0"), ParryMontage); //패리 페널티 3회일 때는 Parry0 섹션재생
            break;
          default:
            AnimInstance->Montage_JumpToSection(FName("Parry0"), ParryMontage); //패리 페널티 3회 이상일 때는 Parry0 섹션재생
            
            break;
            
        }
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
      TWeakObjectPtr<USanzoParryComponent> WeakThis;
      GetWorld()->GetTimerManager().SetTimer(
        SlowTimerHandle,
        [WeakThis]()
        {
          WeakThis->GetWorld()->GetWorldSettings()->SetTimeDilation(1.0f); //시간 정상화
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

