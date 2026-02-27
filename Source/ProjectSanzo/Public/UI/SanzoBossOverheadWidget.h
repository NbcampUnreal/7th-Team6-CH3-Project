// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SanzoBossOverheadWidget.generated.h"

struct FEnemyOverHeadData;

UCLASS()
class PROJECTSANZO_API USanzoBossOverheadWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	
	
	UFUNCTION( BlueprintCallable )
	void UpdateOverHeadWidget(const FEnemyOverHeadData& Data);
	
	
protected:

	UFUNCTION()
	void UpdateStunImage(bool bIsStunned);

	UPROPERTY(meta = (BindWidget))
	class UImage* StateImage;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* StunStateAnim;
	
};
