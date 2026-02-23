// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "Core/SanzoStageTypes.h"
#include "SanzoStageAnnouncerWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAnnouncerEnded);

UCLASS()
class PROJECTSANZO_API USanzoStageAnnouncerWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	void NativeConstruct() override;
	
	void SetStageAnnouncer(FGameplayTag State, ESanzoStageType StageType);

	FOnAnnouncerEnded OnAnnouncerEnded;
	
protected:
	UFUNCTION()
	void SetStageStartUI(ESanzoStageType StageType);
	UFUNCTION()
	void SetStageClearUI();
	
	UFUNCTION()
	void OnAnimationFinishedHandle();
		
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AnnounceText;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SubAnnounceText;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* AnnounceTextAnim;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* SubAnnounceTextAnim;
	
	
};
