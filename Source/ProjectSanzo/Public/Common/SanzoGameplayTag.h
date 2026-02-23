// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"
/**
 * 
 */
namespace SanzoTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Aiming);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sprint);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Exhausted);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Parry);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Dodge);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(IFrame);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Action_Fixed);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ParryWindow);
}
//필요한 태그를 미리 선언	
//사용법 SanzoTags::Aiming, SanzoTags::Sprint, SanzoTags::Attack, SanzoTags::Exhausted



class PROJECTSANZO_API SanzoGameplayTag
{
public:
	SanzoGameplayTag();
	~SanzoGameplayTag();
};
