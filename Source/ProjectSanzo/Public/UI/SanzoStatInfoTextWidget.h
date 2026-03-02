// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/UpgradeSystem/UpgradeDataRow.h"
#include "SanzoStatInfoTextWidget.generated.h"

UCLASS()
class PROJECTSANZO_API USanzoStatInfoTextWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	
	UFUNCTION(BlueprintCallable)
	void SetStatInfoText(FText InName, EStatModifierType InModifierType, float InCurrentValue,float InBaseValue, float InBonusValue);
	
	UFUNCTION(BlueprintCallable)
	void SetBonusValueText(EStatModifierType InModifierType, float InBonusValue);
	
protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* StatName;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CurrentValue;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* BaseValue;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* UpgradeValue;
};
