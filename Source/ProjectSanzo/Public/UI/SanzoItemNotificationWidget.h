// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SanzoItemNotificationWidget.generated.h"

UCLASS()
class PROJECTSANZO_API USanzoItemNotificationWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
	void SetNotify(FName ItemType, int32 Amount);
	
	UFUNCTION()
	void OnNotifyAnimFinished();
	
protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemNotifyText;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* ItemNotifyAnim;
	
};
