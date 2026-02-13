// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "SanzoPlayerController.generated.h"

class USanzoPopUpWidget;
struct FGameplayTag;
class USanzoMainWidget;
class USanzoHUDWidget;
class UInputMappingContext;

UCLASS(abstract)
class PROJECTSANZO_API ASanzoPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	virtual void BeginPlay() override;

#pragma region CameraSettings
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float MaxPitchAngle = 60.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
  float MinPitchAngle = -60.0f;
#pragma endregion 김형백

#pragma region UI

public:
	ASanzoPlayerController();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	TSubclassOf<USanzoHUDWidget> HUDWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HUD")
	USanzoHUDWidget* HUDWidgetInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
	TSubclassOf<USanzoMainWidget> MenuWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Menu")
	USanzoMainWidget* MenuWidgetInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
	TSubclassOf<USanzoPopUpWidget> PopUpWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Menu")
	USanzoPopUpWidget* PopUpWidgetInstance;

	UFUNCTION(BlueprintCallable, Category = "HUD")
	USanzoHUDWidget* GetHUDWidget() const { return HUDWidgetInstance; }

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ShowGameHUD();

	UFUNCTION(BlueprintCallable, Category = "PopUp")
	USanzoPopUpWidget* GetPopUpWidget() const { return PopUpWidgetInstance; }

	UFUNCTION(BlueprintCallable, Category = "PopUp")
	void ShowPopUp(FGameplayTag State);
	UFUNCTION(BlueprintCallable, Category = "PopUp")
	void ResumeGame();

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void ShowMainUI(FGameplayTag State);
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void StartGame();
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void ReturnMainMenu();
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void ExitGame();

	//Tag
	UPROPERTY(EditDefaultsOnly, Category = "State")
	FGameplayTag MainMenuTag;
	UPROPERTY(EditDefaultsOnly, Category = "State")
	FGameplayTag StageClearedTag;
	UPROPERTY(EditDefaultsOnly, Category = "State")
	FGameplayTag GameOverTag;
	UPROPERTY(EditDefaultsOnly, Category = "State")
	FGameplayTag GamePauseTag;
	UPROPERTY(EditDefaultsOnly, Category = "State")
	FGameplayTag GameUpgradeTag;
	UPROPERTY(EditDefaultsOnly, Category = "State")
	FGameplayTag GamePlayingTag;

#pragma endregion 이준로
};
