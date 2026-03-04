// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "SanzoMediaPlayerWidget.generated.h"

USTRUCT(BlueprintType)
struct FMediaSubtitleData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeStamp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = true))
	FText SubtitleText;
};

USTRUCT(BlueprintType)
struct FMediaSubTitleList
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FMediaSubtitleData> Subtitles;
};

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

	UFUNCTION()
	void OnMediaEndReached();

	void FinishedMediaScene();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sanzo|Resources")
	UMediaPlayer* CommonMediaPlayer;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sanzo|Time")
  float OpeningTextBoxDisappearStartTime;
  
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sanzo|Time")
  float EndingTextBoxDisappearStartTime;
  
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

#pragma region Subtitles

protected:
	UFUNCTION()
	void UpdateSubtitle();

private:
	UPROPERTY(EditAnywhere, Category= "Sanzo|Data")
	TMap<FGameplayTag, FMediaSubTitleList> SubtitleData;

	FGameplayTag CurrentPlayingTag;

	FTimerHandle SubtitlesTimerHandle;
	float CurrentSubtitleIndex;
  
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* DisappearTextBoxAnim;
  
  UPROPERTY(Transient, meta = (BindWidgetAnim))
  class UWidgetAnimation* DisappearEndingTextBoxAnim;

#pragma endregion 이준로

#pragma region Skip

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDT) override;

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	FTimerHandle SkipTimerHandle;

	UFUNCTION()
	void UpdateSkipProgress(float NewPercent);

	void ExecuteSkip();

	bool IsAnyKeyPressed = false;

	float PressStartTime = 0.0f;
	
	UPROPERTY(EditAnywhere, Category= "Sanzo|Data")
	float TimeToSkip = 2.0f;

	UPROPERTY()
	UMaterialInstanceDynamic* SkipProgressBarDynamic;

	UPROPERTY(meta = (BindWidget))
	class UImage* MediaSkipProgressBar;

#pragma endregion 이준로
};
