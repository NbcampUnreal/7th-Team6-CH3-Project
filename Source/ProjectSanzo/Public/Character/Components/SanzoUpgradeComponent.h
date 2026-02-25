// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/UpgradeSystem/UpgradeDataRow.h"
#include "Character/Interface/SanzoUpgradeInterface.h"
#include "SanzoUpgradeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTSANZO_API USanzoUpgradeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	USanzoUpgradeComponent();

protected:
	
	virtual void BeginPlay() override;
	
	void OnNewUpgradeSelected(const FUpgradeOption& UpgradeOption);

	void ProcessUpgradeValue(EUpgradeTarget Target, EUpgradeType Type, float TotalValue);
	
	void InitializeUpgradeFromSubsystem(const TMap<FUpgradeStatKey, float> TotalMap);
	
		
};
