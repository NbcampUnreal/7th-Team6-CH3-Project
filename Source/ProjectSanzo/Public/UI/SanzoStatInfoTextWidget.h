// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SanzoStatInfoTextWidget.generated.h"

UCLASS()
class PROJECTSANZO_API USanzoStatInfoTextWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	
	UFUNCTION(BlueprintCallable)
	void SetStatInfoText(FText InName, float InCurrentValue,float InBaseValue, float InBonusValue);
	
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
