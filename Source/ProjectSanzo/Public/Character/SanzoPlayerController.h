// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Core/SanzoStageTypes.h"
#include "GameFramework/PlayerController.h"
#include "SanzoPlayerController.generated.h"

class USanzoMediaPlayerWidget;
class USanzoStageAnnouncerWidget;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Announcer")
	TSubclassOf<USanzoStageAnnouncerWidget> StageAnnouncerWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Announcer")
	USanzoStageAnnouncerWidget* StageAnnouncerWidgetInstance;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MediaPlayer")
	TSubclassOf<USanzoMediaPlayerWidget> MediaPlayerWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MediaPlayer")
	USanzoMediaPlayerWidget* MediaPlayerWidgetInstance;
	
	UFUNCTION(BlueprintCallable, Category = "HUD")
	USanzoHUDWidget* GetHUDWidget() const { return HUDWidgetInstance; }

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ShowGameHUD();

	UFUNCTION(BlueprintCallable, Category = "PopUp")
	USanzoPopUpWidget* GetPopUpWidget() const { return PopUpWidgetInstance; }

	UFUNCTION(BlueprintCallable, Category = "PopUp")
	void ShowPopUp(FGameplayTag State);
	UFUNCTION(BlueprintCallable, Category = "PopUp")
	void OnPopUpClosed(FGameplayTag State);
	UFUNCTION(BlueprintCallable, Category = "PopUp")
	void ResumeGame();
	
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void ShowMainUI(FGameplayTag State, float ClearTime = 0.0f, int32 KillScore = 0);
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void OnMainClosed(FGameplayTag State);
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void StartGame();
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void ReturnMainMenu();
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void QuitGame();

	UFUNCTION(BlueprintCallable, Category = "Announcer")
	void ShowAnnouncerUI(FGameplayTag State, ESanzoStageType StageType = ESanzoStageType::None);
	UFUNCTION(BlueprintCallable, Category = "Announcer")
	void AnnounceEnded();
	
	UFUNCTION(BlueprintCallable, Category = "MediaPlayer")
	void ShowMediaPlayer(FGameplayTag State);
	UFUNCTION(BlueprintCallable, Category = "MediaPlayer")
	void MediaSceneFinished();
	
	//Tag
	UPROPERTY(EditDefaultsOnly, Category = "State")
	FGameplayTag MainMenuTag;
	UPROPERTY(EditDefaultsOnly, Category = "State")
	FGameplayTag StageClearedTag;
	UPROPERTY(EditDefaultsOnly, Category = "State")
	FGameplayTag GameOverTag;
	UPROPERTY(EditDefaultsOnly, Category = "State")
	FGameplayTag QuitGameTag;
	UPROPERTY(EditDefaultsOnly, Category = "State")
	FGameplayTag GamePauseTag;
	UPROPERTY(EditDefaultsOnly, Category = "State")
	FGameplayTag GameUpgradeTag;
	UPROPERTY(EditDefaultsOnly, Category = "State")
	FGameplayTag GamePlayingTag;
	UPROPERTY(VisibleAnywhere, Category = "State")
	FGameplayTag GameOpening;
	
#pragma endregion 이준로
};
