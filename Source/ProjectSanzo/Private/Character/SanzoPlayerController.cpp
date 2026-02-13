// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/SanzoPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "UI/SanzoMainWidget.h"
#include "UI/SanzoHUDWidget.h"
#include "UI/SanzoPopUpWidget.h"
#include "Core/SanzoGameInstance.h"
#include "Core/SanzoGameState.h"
#include "Engine/LocalPlayer.h"
#include "Kismet/GameplayStatics.h"

ASanzoPlayerController::ASanzoPlayerController()
{
	HUDWidgetClass = nullptr;
	HUDWidgetInstance = nullptr;
	MenuWidgetClass = nullptr;
	MenuWidgetInstance = nullptr;
	PopUpWidgetClass = nullptr;
	PopUpWidgetInstance = nullptr;
}

void ASanzoPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		GetLocalPlayer()))
	{
		if (DefaultMappingContext)
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

  PlayerCameraManager->ViewPitchMax = MaxPitchAngle;
  PlayerCameraManager->ViewPitchMin = MinPitchAngle;

	FString CurrentMapName = GetWorld()->GetMapName();
	if (CurrentMapName.Contains("L_MainMenu"))
	{
		ShowMainUI(MainMenuTag);
	}
}

#pragma region UI

void ASanzoPlayerController::ShowGameHUD()
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (MenuWidgetInstance)
	{
		MenuWidgetInstance->RemoveFromParent();
		MenuWidgetInstance = nullptr;
	}

	if (HUDWidgetClass)
	{
		HUDWidgetInstance = CreateWidget<USanzoHUDWidget>(this, HUDWidgetClass);
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();

			bShowMouseCursor = false;
			SetInputMode(FInputModeGameOnly());
		}
	}
}

void ASanzoPlayerController::ShowPopUp(FGameplayTag State)
{
	if (PopUpWidgetInstance)
	{
		return;
	}
	if (PopUpWidgetClass)
	{
		PopUpWidgetInstance = CreateWidget<USanzoPopUpWidget>(this, PopUpWidgetClass);
		{
			if (PopUpWidgetInstance)
			{
				PopUpWidgetInstance->AddToViewport();

				SetPause(true);
				bShowMouseCursor = true;
				SetInputMode(FInputModeUIOnly());
			}
		}
		if (State == GameUpgradeTag)
		{
			PopUpWidgetInstance->SetUpgradeUI();
		}
		if (State == GamePauseTag)
		{
			PopUpWidgetInstance->SetPauseUI();
		}
	}
}

void ASanzoPlayerController::ResumeGame()
{
	if (PopUpWidgetInstance)
	{
		PopUpWidgetInstance->RemoveFromParent();
		PopUpWidgetInstance = nullptr;
	}

	SetPause(false);
	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());
}

void ASanzoPlayerController::ShowMainUI(FGameplayTag State)
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (MenuWidgetInstance)
	{
		MenuWidgetInstance->RemoveFromParent();
		MenuWidgetInstance = nullptr;
	}

	if (MenuWidgetClass)
	{
		MenuWidgetInstance = CreateWidget<USanzoMainWidget>(this, MenuWidgetClass);
		if (MenuWidgetInstance)
		{
			MenuWidgetInstance->AddToViewport();

			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());
		}

		if (State == MainMenuTag)
		{
			//메인 메뉴
			MenuWidgetInstance->SetMainMenuUI();
		}
		if (State == StageClearedTag)
		{
			//스테이지 클리어
			//이후 StageManger 또는 GameState에서 정보 받아서 구현 필요.
			MenuWidgetInstance->SetStageClearMenuUI(10.23f, 30);
		}
		if (State == GameOverTag)
		{
			//게임 오버
			MenuWidgetInstance->SetGameOverMenuUI();
		}
	}
}

void ASanzoPlayerController::StartGame()
{
	if (USanzoGameInstance* SanzoGameInstance = Cast<USanzoGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		//게임 시작시 초기화 필요한 값 입력
		SanzoGameInstance->CurrentStageIndex = 0;
		SanzoGameInstance->MoveToNextStage();
	}
	SetPause(false);
}

void ASanzoPlayerController::ReturnMainMenu()
{
	if (PopUpWidgetInstance)
	{
		PopUpWidgetInstance->RemoveFromParent();
		PopUpWidgetInstance = nullptr;
	}

	UGameplayStatics::OpenLevel(GetWorld(), FName("L_MainMenu"));
	SetPause(true);
}

void ASanzoPlayerController::ExitGame()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	UKismetSystemLibrary::QuitGame(
		World,
		this,
		EQuitPreference::Quit,
		false
	);
}
#pragma endregion 이준로
