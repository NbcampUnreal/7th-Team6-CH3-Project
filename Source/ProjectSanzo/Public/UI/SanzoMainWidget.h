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
	class UButton* StartButton;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* StartText;
	
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
};
