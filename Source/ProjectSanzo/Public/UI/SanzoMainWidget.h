// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SanzoMainWidget.generated.h"

UCLASS()
class PROJECTSANZO_API USanzoMainWidget : public UUserWidget
{
	GENERATED_BODY()
	
#pragma region UI

public:
	UFUNCTION()
	void SetMainMenuUI();

	UFUNCTION()
	void SetStageClearMenuUI(float ClearTime, int32 KillScroe);

	UFUNCTION()
	void SetGameOverMenuUI();
	
	UPROPERTY(meta = (BindWidget))
	class UImage* TitleImage;
	
	UPROPERTY(meta = (BindWidget))
	class UBorder* BackBoard;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* StartButton;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* StartText;
	
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
	
#pragma endregion 이준로
};
