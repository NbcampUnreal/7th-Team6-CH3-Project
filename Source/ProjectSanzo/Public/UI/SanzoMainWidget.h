// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SanzoPopUpWidget.h"
#include "Blueprint/UserWidget.h"
#include "SanzoMainWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMainUIButtonClicked, FGameplayTag, CurrentState);

UCLASS()
class PROJECTSANZO_API USanzoMainWidget : public UUserWidget
{
	GENERATED_BODY()
	
#pragma region UI

public:
	
	virtual void NativeConstruct() override;
	
	virtual void NativeDestruct() override;
	
	UFUNCTION()
	void SetMainUI(FGameplayTag State, float ClearTime, int32 KillScore);
	
	FOnMainUIButtonClicked OnButtonClicked;
	
protected:
	
	UFUNCTION()
	void SetMainMenuUI();

	UFUNCTION()
	void SetStageClearMenuUI(float ClearTime, int32 KillScore);

	UFUNCTION()
	void SetGameOverMenuUI();
	
	UFUNCTION()
	void HandleStartButtonClicked();
	
	UFUNCTION()
	void HandleExitButtonClicked();
	
	UPROPERTY(meta = (BindWidget))
	class UImage* TitleImage;
	
	UPROPERTY(meta = (BindWidget))
	class UBorder* BackBoard;
	
	UPROPERTY(meta = (BindWidget))
	class UOverlay* StartButtonOverlay;
	UPROPERTY(meta = (BindWidget))
	class UButton* StartButton;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* StartText;
	
	UPROPERTY(meta = (BindWidget))
	class UOverlay* ExitButtonOverlay;
	UPROPERTY(meta = (BindWidget))
	class UButton* ExitButton;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock*  ExitText;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TitleText;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SubTitleText;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ClearTimeText;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* KillScoreText;
	
	//Tag
	UPROPERTY(VisibleAnywhere)
	FGameplayTag CurrentState;
	
	UPROPERTY(VisibleAnywhere)
	FGameplayTag StartGameTag;
	
	UPROPERTY(VisibleAnywhere)
	FGameplayTag QuitGameTag;
	
	UPROPERTY(VisibleAnywhere)
	FGameplayTag NextStageTag;
	
	UPROPERTY(VisibleAnywhere)
	FGameplayTag ReturnMainMenuTag;
	
#pragma endregion 이준로
	
#pragma region GameOverUI
protected:
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* GameOverAnim;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Sound")
	USoundBase* GameOverSound;
	
	UPROPERTY()
	UAudioComponent* PlayingAudio;
	
	UFUNCTION()
	void PlayGameOverSound();
	
#pragma endregion 이준로
	
#pragma region ButtonSound
	
public:
	
	UFUNCTION()
	void ExecuteStartTransition();
	UFUNCTION()
	void ExecuteExitTransition();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Sound")
	USoundBase* StartSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Sound")
	USoundBase* QuitSound;
	
  float ExecuteTransitionTargetTime = 0;
  float ExecuteTransitionDelay = 0;
  
  bool bIsStartButtonTransitioning = false;
  bool bIsExitButtonTransitioning = false;
  
  virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
  
#pragma endregion 이준로
	
#pragma region StageClearUI
	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Sound")
	USoundBase* StageClearSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Sound")
	USoundBase* ResultSound;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* StageClearAnim;
	
	
#pragma endregion 이준로
  
#pragma region MainMenuUI
  
protected:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Sound")
  USoundBase* MainMenuSound;
  
  UFUNCTION()
  void PlayMainMenuSound();
#pragma endregion 이준로
};
