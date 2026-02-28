// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/UpgradeSystem/UpgradeDataRow.h"
#include "SanzoStatInfoWidget.generated.h"

UCLASS()
class PROJECTSANZO_API USanzoStatInfoWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION()
	void SetStatInfo(TArray<FStatusDisplayData> CurrentStatData, const TMap<FUpgradeStatKey, float>& UpgradeTotalMap);

protected:
	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<class UUserWidget> StatInfoTextWidgetClass;
	
	UPROPERTY(meta = (BindWidget))
	class UScrollBox* StatScrollBox;
	
	UPROPERTY(meta = (BindWidget))
	class UScrollBox* GunScrollBox;
	
	UPROPERTY(meta = (BindWidget))
	class UScrollBox* BowScrollBox;
	
};
