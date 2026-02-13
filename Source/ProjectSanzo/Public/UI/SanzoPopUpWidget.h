// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SanzoPopUpWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSANZO_API USanzoPopUpWidget : public UUserWidget
{
	GENERATED_BODY()
#pragma region PopUpUI

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void SetPauseUI();

	UFUNCTION(BlueprintCallable)
	void SetUpgradeUI();

protected:
	UFUNCTION(BlueprintCallable)
	void SetStatusText();

	UFUNCTION(BlueprintCallable)
	void SetUpgradeListText();

	UFUNCTION(BlueprintCallable)
	void SetUpgradeButton(int32 index);

	UFUNCTION()
	FLinearColor GetColorByRarity(int32 Value);

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PopUpText;

	UPROPERTY(meta = (BindWidget))
	class UBorder* UpgradeListBorder;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* UpgradeListText;

	UPROPERTY(meta = (BindWidget))
	class UButton* ResumeButton;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ResumeText;

	UPROPERTY(meta = (BindWidget))
	class UButton* MainMenuButton;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MainMenuText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* StatusText;

	UPROPERTY(meta = (BindWidget))
	class UButton* UpgradeButton_1;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* UpgradeText_1;

	UPROPERTY(meta = (BindWidget))
	class UButton* UpgradeButton_2;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* UpgradeText_2;

	UPROPERTY(meta = (BindWidget))
	class UButton* UpgradeButton_3;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* UpgradeText_3;

	UPROPERTY()
	TArray<UButton*> UpgradeButtons;
	UPROPERTY()
	TArray<UTextBlock*> UpgradeTexts;

#pragma endregion 이준로
};
