// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "SanzoANS_ParryResponse.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSANZO_API USanzoANS_ParryResponse : public UAnimNotifyState
{
	GENERATED_BODY()
	// 구간이 시작될 때 1회 호출
	virtual void NotifyBegin(
		USkeletalMeshComponent* MeshComp, 
		UAnimSequenceBase* Animation, 
		float TotalDuration, 
		const FAnimNotifyEventReference& EventReference) override;

	// 구간 동안 매 프레임 호출
	virtual void NotifyTick(
		USkeletalMeshComponent* MeshComp, 
		UAnimSequenceBase* Animation, 
		float FrameDeltaTime, 
		const FAnimNotifyEventReference& EventReference) override;

	// 구간이 끝날 때 1회 호출
	virtual void NotifyEnd(
		USkeletalMeshComponent* MeshComp, 
		UAnimSequenceBase* Animation, 
		const FAnimNotifyEventReference& EventReference) override;
	
};
