// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/SanzoPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Common/SanzoLog.h"
#include "UI/SanzoMainWidget.h"
#include "UI/SanzoHUDWidget.h"
#include "UI/SanzoPopUpWidget.h"
#include "UI/SanzoStageAnnouncerWidget.h"
#include "Core/SanzoGameInstance.h"
#include "Core/SanzoGameMode.h"
#include "Core/SanzoGameState.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "UI/SanzoMediaPlayerWidget.h"

ASanzoPlayerController::ASanzoPlayerController()
{
	HUDWidgetClass = nullptr;
	HUDWidgetInstance = nullptr;
	MenuWidgetClass = nullptr;
	MenuWidgetInstance = nullptr;
	PopUpWidgetClass = nullptr;
	PopUpWidgetInstance = nullptr;
	MediaPlayerWidgetClass = nullptr;
	MediaPlayerWidgetInstance = nullptr;
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

#pragma region InitUI
	
	USanzoGameInstance* GameInstance = Cast<USanzoGameInstance>(GetGameInstance());
	if (!GameInstance) return;
	
	FString CurrentMapName = GetWorld()->GetMapName();
	if (CurrentMapName.Contains("L_MainMenu"))
	{
		FGameplayTag VideoToPlay = GameInstance->MediaPlayTag;
		if (VideoToPlay.IsValid())
		{
			if (!VideoToPlay.MatchesTag(FGameplayTag::RequestGameplayTag(FName("UI.State.None"))))
			{
				UE_LOG(LogLJR, Warning, TEXT("영상 재생 태그: %s"), *VideoToPlay.ToString());
				ShowMediaPlayer(VideoToPlay);
			}
			else
			{
				ShowMainUI(MainMenuTag);
			}
		}
		
		GameInstance->MediaPlayTag = FGameplayTag::RequestGameplayTag(FName("UI.State.None"));		
	}
#pragma endregion 이준로
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
				PopUpWidgetInstance->OnButtonClicked.AddDynamic(this, &ThisClass::OnPopUpClosed);
				
				PopUpWidgetInstance->AddToViewport();

				SetPause(true);
				bShowMouseCursor = true;
				SetInputMode(FInputModeUIOnly());
			}
			
			PopUpWidgetInstance->SetPopUpUI(State);
		}
	}
}

void ASanzoPlayerController::OnPopUpClosed(FGameplayTag State)
{
	if (State == MainMenuTag)
	{
		ReturnMainMenu();
	}
	if (State == GamePlayingTag)
	{
		ResumeGame();
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

void ASanzoPlayerController::ShowMainUI(FGameplayTag State, float ClearTime, int32 KillScore)
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
			MenuWidgetInstance->OnButtonClicked.AddDynamic(this, &ThisClass::OnMainClosed);
			MenuWidgetInstance->AddToViewport();
			if (State != GameOverTag)
			{
				SetPause(true);
			}
			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());
		}
		MenuWidgetInstance->SetMainUI(State, ClearTime, KillScore);
	}
}

void ASanzoPlayerController::OnMainClosed(FGameplayTag State)
{
	if (State == MainMenuTag)
	{
		ReturnMainMenu();
	}
	if (State == GamePlayingTag)
	{
		StartGame();
	}
	if (State == QuitGameTag)
	{
		QuitGame();
	}
	if (State == FGameplayTag::RequestGameplayTag(FName("Room.State.MoveNext")))
	{
		AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();
		if (GameMode)
		{
			ASanzoGameMode* SanzoGameMode = Cast<ASanzoGameMode>(GameMode);
			if (SanzoGameMode)
			{
				SanzoGameMode->MoveToNextStage();
			}
		}
	}
}

void ASanzoPlayerController::StartGame()
{
	if (USanzoGameInstance* SanzoGameInstance = Cast<USanzoGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		//게임 시작시 초기화 필요한 값 입력
		SanzoGameInstance->InitSetup();
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

void ASanzoPlayerController::QuitGame()
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

void ASanzoPlayerController::ShowAnnouncerUI(FGameplayTag State, ESanzoStageType StageType)
{
	if (MenuWidgetInstance)
	{
		MenuWidgetInstance->RemoveFromParent();
		MenuWidgetInstance = nullptr;
	}
	
	if (StageAnnouncerWidgetClass)
	{
		StageAnnouncerWidgetInstance = CreateWidget<USanzoStageAnnouncerWidget>(this, StageAnnouncerWidgetClass);
		if (StageAnnouncerWidgetInstance)
		{
			StageAnnouncerWidgetInstance->OnAnnouncerEnded.AddDynamic(this,&ThisClass::AnnounceEnded);
			StageAnnouncerWidgetInstance->AddToViewport();
			
			if (State == FGameplayTag::RequestGameplayTag(FName("Room.State.Actived")))
			{
				SetPause(true);
			}
			
			if (State == FGameplayTag::RequestGameplayTag(FName("Room.State.Cleared")))
			{
				SetPause(false);
			}
		}
		
	}
	StageAnnouncerWidgetInstance->SetStageAnnouncer(State, StageType);
	
}

void ASanzoPlayerController::AnnounceEnded()
{
	if (StageAnnouncerWidgetInstance)
	{
		StageAnnouncerWidgetInstance->RemoveFromParent();
		StageAnnouncerWidgetInstance = nullptr;
	}
	
	if (PopUpWidgetInstance)
	{
		return;
	}
	
	SetPause(false);
}

void ASanzoPlayerController::ShowMediaPlayer(FGameplayTag State)
{
	if (MenuWidgetInstance)
	{
		MenuWidgetInstance->RemoveFromParent();
		MenuWidgetInstance = nullptr;
	}
	
	if (PopUpWidgetInstance)
	{
		PopUpWidgetInstance->RemoveFromParent();
		PopUpWidgetInstance = nullptr;
	}
	
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}
	
	if (MediaPlayerWidgetClass)
	{
		MediaPlayerWidgetInstance = CreateWidget<USanzoMediaPlayerWidget>(this,MediaPlayerWidgetClass);
		if (MediaPlayerWidgetInstance)
		{
			MediaPlayerWidgetInstance->OnMediaSceneFinished.AddDynamic(this, &ASanzoPlayerController::MediaSceneFinished);
			
			MediaPlayerWidgetInstance->AddToViewport();
			MediaPlayerWidgetInstance->PlayMediaScene(State);
			
			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(MediaPlayerWidgetInstance->TakeWidget()); // 위젯에 강제 포커스
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
			
			SetInputMode(InputMode);	
			bShowMouseCursor = false;
		}
	}
}

void ASanzoPlayerController::MediaSceneFinished()
{
	ShowMainUI(MainMenuTag);
}
#pragma endregion 이준로
