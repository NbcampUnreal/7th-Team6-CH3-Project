// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Core/UpgradeSystem/UpgradeDataRow.h"
#include "Blueprint/UserWidget.h"
#include "SanzoPopUpWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPopUpUIButtonClicked, FGameplayTag, CurrentState);

UCLASS()
class PROJECTSANZO_API USanzoPopUpWidget : public UUserWidget
{
	GENERATED_BODY()
#pragma region PopUpUI

public:

	virtual void NativeConstruct() override;
	
	void SetPopUpUI(FGameplayTag GameState);
	
	UPROPERTY()
	TArray<FUpgradeOption> CurrentOptions;
	
	FOnPopUpUIButtonClicked OnButtonClicked;
	
protected:
	
	UFUNCTION(BlueprintCallable)
	void SetPauseUI();

	UFUNCTION(BlueprintCallable)
	void SetUpgradeUI();
	
	UPROPERTY(VisibleAnywhere)
	FGameplayTag CurrentState;
	
	UFUNCTION(BlueprintCallable)
	void SetStatusText();

	UFUNCTION(BlueprintCallable)
	void SetUpgradeList();

	UFUNCTION(BlueprintCallable)
	void SetUpgradeButton(int32 index, const FUpgradeOption& option);
	
	UFUNCTION()
	void HandleUpgradeSelected(const FUpgradeOption& SelectedUpgrade);

	UFUNCTION()
	void HandleMenuButtonClicked();
	
	UFUNCTION()
	void HandleResumeButtonClicked();

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PopUpText;

	UPROPERTY(meta = (BindWidget))
	class UBorder* UpgradeListBorder;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* UpgradeListText;
	UPROPERTY(meta = (BindWidget))
	class UWrapBox* UpgradeListWrapBox;
	
	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<class UUserWidget> UpgradeInfoWidgetClass;

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
	class UVerticalBox* UpgradeButtonBox;

	UPROPERTY(meta = (BindWidget))
	class USanzoUpgradeButtonWidget* UpgradeButton_1;
	UPROPERTY(meta = (BindWidget))
	class USanzoUpgradeButtonWidget* UpgradeButton_2;
	UPROPERTY(meta = (BindWidget))
	class USanzoUpgradeButtonWidget* UpgradeButton_3;

	UPROPERTY()
	TArray<USanzoUpgradeButtonWidget*> UpgradeButtons;
	
	//Tag
	UPROPERTY(EditDefaultsOnly, Category = "State")
	FGameplayTag GamePlayingTag;
	UPROPERTY(EditDefaultsOnly, Category = "State")
	FGameplayTag MainMenuTag;
	
#pragma endregion 이준로
};
