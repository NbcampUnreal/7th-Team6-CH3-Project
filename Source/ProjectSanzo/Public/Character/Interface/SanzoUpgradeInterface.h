// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Core/UpgradeSystem/UpgradeDataRow.h" //전방선언 해도됨
#include "SanzoUpgradeInterface.generated.h"


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USanzoUpgradeInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTSANZO_API ISanzoUpgradeInterface
{
	GENERATED_BODY()
public:
	virtual void ApplyUpgrade(EUpgradeTarget Target, EUpgradeType Type, float Value) = 0;
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
};
