// Fill out your copyright notice in the Description page of Project Settings.


#include "Notifies/SanzoANS_ParryResponse.h"
#include "Common/SanzoGameplayTag.h"
#include "GameplayTagAssetInterface.h"

void USanzoANS_ParryResponse::NotifyBegin(
  USkeletalMeshComponent* MeshComp, 
  UAnimSequenceBase* Animation, 
  float TotalDuration, 
  const FAnimNotifyEventReference& EventReference)
{
  IGameplayTagAssetInterface* GameplayTagInterface = Cast<IGameplayTagAssetInterface>(MeshComp->GetOwner());
  if(GameplayTagInterface)
  {
    if (GameplayTagInterface->HasMatchingGameplayTag(SanzoTags::Parrying))
    {
      //패리 태그가 있다면 패리 성공
      UE_LOG(LogTemp, Log, TEXT("Parry Success!"));
    }
    else
    {
      //패리 태그가 없다면 패리 실패
      UE_LOG(LogTemp, Log, TEXT("Parry Failed!"));
    }
  }

  
}

void USanzoANS_ParryResponse::NotifyTick(
  USkeletalMeshComponent* MeshComp, 
  UAnimSequenceBase* Animation, 
  float FrameDeltaTime, 
  const FAnimNotifyEventReference& EventReference)
{
}

void USanzoANS_ParryResponse::NotifyEnd(
  USkeletalMeshComponent* MeshComp, 
  UAnimSequenceBase* Animation, 
  const FAnimNotifyEventReference& EventReference)
{
}
