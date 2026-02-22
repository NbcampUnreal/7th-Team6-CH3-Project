// Fill out your copyright notice in the Description page of Project Settings.


#include "Notifies/SanzoANS_ParryResponse.h"
#include "Common/SanzoGameplayTag.h"
#include "GameplayTagAssetInterface.h"
#include "Character/Interface/SanzoTagEditorInterface.h"

void USanzoANS_ParryResponse::NotifyBegin(
  USkeletalMeshComponent* MeshComp,
  UAnimSequenceBase* Animation,
  float TotalDuration,
  const FAnimNotifyEventReference& EventReference)
{
  if (ISanzoTagEditorInterface* TagEditorInterface = Cast<ISanzoTagEditorInterface>(MeshComp->GetOwner()))
  {
    TagEditorInterface->AddGameplayTag(SanzoTags::ParryWindow);
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
  if (ISanzoTagEditorInterface* TagEditorInterface = Cast<ISanzoTagEditorInterface>(MeshComp->GetOwner()))
  {
    TagEditorInterface->RemoveGameplayTag(SanzoTags::ParryWindow);
  }
}
