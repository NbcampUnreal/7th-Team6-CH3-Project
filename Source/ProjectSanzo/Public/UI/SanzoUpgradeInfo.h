// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/UpgradeSystem/UpgradeDataRow.h"
#include "SanzoUpgradeInfo.generated.h"

UCLASS()
class PROJECTSANZO_API USanzoUpgradeInfo : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION( BlueprintCallable )
	void SetUpgradeInfoBlock(FName UpgradeID, int32 Count);

protected:
	UFUNCTION()
	void SetIconBackground(EUpgradeRarity UpgradeRarity);
	
	UPROPERTY(EditAnywhere, Category = "Upgrade System|DataTable")
	UDataTable* UpgradeDataTable;
	
private:
	UPROPERTY(EditAnywhere, Category = "Upgrade System|Background Images")
	TMap<EUpgradeRarity, UTexture2D*> IconBackgrounds;
	
	UPROPERTY(meta = (BindWidget))
	class UImage* IconBackground;
	UPROPERTY(meta = (BindWidget))
	class UImage* IconImage;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* UpgradeCount;
};
