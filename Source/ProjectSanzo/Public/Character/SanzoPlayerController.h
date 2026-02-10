// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SanzoPlayerController.generated.h"

class UInputMappingContext;

UCLASS(abstract)
class PROJECTSANZO_API ASanzoPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	virtual void BeginPlay() override;

#pragma region UI

public:
	ASanzoPlayerController();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	TSubclassOf<UUserWidget> HUDWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HUD")
	UUserWidget* HUDWidgetInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
	TSubclassOf<UUserWidget> MenuWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Menu")
	UUserWidget* MenuWidgetInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
	TSubclassOf<UUserWidget> PopUpWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Menu")
	UUserWidget* PopUpWidgetInstance;


	UFUNCTION(BlueprintCallable, Category = "HUD")
	UUserWidget* GetHUDWidget() const { return HUDWidgetInstance; }

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ShowGameHUD();

	UFUNCTION(BlueprintCallable, Category = "PopUp")
	UUserWidget* GetPopUpWidget() const { return PopUpWidgetInstance; }

	UFUNCTION(BlueprintCallable, Category = "PopUp")
	void ShowPopUp(bool bIsLevelUp);
	UFUNCTION(BlueprintCallable, Category = "PopUp")
	void ResumeGame();

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void ShowMainUI(int32 CaseIndex);
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void StartGame();
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void ReturnMainMenu();
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void ExitGame();


#pragma endregion 이준로
};
