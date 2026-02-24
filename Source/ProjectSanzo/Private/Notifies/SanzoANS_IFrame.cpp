// Fill out your copyright notice in the Description page of Project Settings.


#include "Notifies/SanzoANS_IFrame.h"
#include "Character/Interface/SanzoTagEditorInterface.h"
#include "Common/SanzoGameplayTag.h"

void USanzoANS_IFrame::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
  if (ISanzoTagEditorInterface* TagEditorInterface = Cast<ISanzoTagEditorInterface>(MeshComp->GetOwner()))
  {
    TagEditorInterface->AddGameplayTag(SanzoTags::IFrame);
  }
}

void USanzoANS_IFrame::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{

}

void USanzoANS_IFrame::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
  if (ISanzoTagEditorInterface* TagEditorInterface = Cast<ISanzoTagEditorInterface>(MeshComp->GetOwner()))
  {
    TagEditorInterface->RemoveGameplayTag(SanzoTags::IFrame);
  }
}
