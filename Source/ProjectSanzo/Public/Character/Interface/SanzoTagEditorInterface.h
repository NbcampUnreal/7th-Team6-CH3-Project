// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "SanzoTagEditorInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USanzoTagEditorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTSANZO_API ISanzoTagEditorInterface
{
	GENERATED_BODY()
public:
	virtual void AddGameplayTag(FGameplayTag TagToAdd) = 0;
	virtual void RemoveGameplayTag(FGameplayTag TagToRemove) = 0;
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
};
