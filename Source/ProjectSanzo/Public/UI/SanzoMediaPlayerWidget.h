// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "SanzoMediaPlayerWidget.generated.h"

class UMediaSource;
class UMediaPlayer;
class UMediaSoundComponent;
class UMediaTexture;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMediaSceneFinished);

UCLASS()
class PROJECTSANZO_API USanzoMediaPlayerWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category="Sanzo|Media")
	void PlayMediaScene(FGameplayTag State);
	
	UPROPERTY()
	FOnMediaSceneFinished OnMediaSceneFinished;
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
	UFUNCTION()
	void OnMediaEndReached();
	
	void FinishedMediaScene();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sanzo|Resources")
	UMediaPlayer* CommonMediaPlayer;
	
	UPROPERTY()
	UMediaSoundComponent* MediaSoundComponent;
	
private:
	UPROPERTY(meta = (BindWidget))
	class UImage* MediaDisplayImage;
	
	UPROPERTY(meta = (BindWidget))
	class UOverlay* StoryTextOverlay;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* StoryText;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SkipText;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* SkipTextAnim;
	
	UPROPERTY(EditAnywhere, Category= "Sanzo|Data")
	TMap<FGameplayTag, TSoftObjectPtr<UMediaSource>> MediaSourceArray;
		
};
