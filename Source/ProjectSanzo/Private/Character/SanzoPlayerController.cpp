// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/SanzoPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
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
	FString CurrentMapName = GetWorld()->GetMapName();
	if (CurrentMapName.Contains("L_MainMenu"))
	{
		ShowMainUI(0);
	}
}

#pragma region UI
UUserWidget* ASanzoPlayerController::GetHUDWidget() const
{
	return HUDWidgetInstance;
}

void ASanzoPlayerController::ShowGameHUD()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Start ShowGameHUD"));
	}
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
		HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		if (HUDWidgetInstance)
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Found HUD"));
			}
			HUDWidgetInstance->AddToViewport();

			bShowMouseCursor = false;
			SetInputMode(FInputModeGameOnly());
		}

		ASanzoGameState* SanzoGameState = GetWorld() ? GetWorld()->GetGameState<ASanzoGameState>() : nullptr;
		if (SanzoGameState)
		{
			SanzoGameState->UpdateHUD();
		}
	}
}

void ASanzoPlayerController::ShowMainUI(int32 CaseIndex)
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
		MenuWidgetInstance = CreateWidget<UUserWidget>(this, MenuWidgetClass);
		if (MenuWidgetInstance)
		{
			MenuWidgetInstance->AddToViewport();

			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());
		}
		switch (CaseIndex)
		{
		case 0:
			//메인 메뉴
			if (UBorder* BackBoard = Cast<UBorder>(MenuWidgetInstance->GetWidgetFromName("BackBoard")))
			{
				BackBoard->SetVisibility(ESlateVisibility::Hidden);
			}
			if (UTextBlock* ButtonText = Cast<UTextBlock>(MenuWidgetInstance->GetWidgetFromName(TEXT("StartButton"))))
			{
				ButtonText->SetText(FText::FromString(TEXT("게임 시작")));
			}
			if (UTextBlock* ButtonText = Cast<UTextBlock>(MenuWidgetInstance->GetWidgetFromName(TEXT("ExitButton"))))
			{
				ButtonText->SetText(FText::FromString(TEXT("게임 종료")));
			}
			break;
		case 1:
			//스테이지 클리어
			if (UImage* TitleImage = Cast<UImage>(MenuWidgetInstance->GetWidgetFromName("TitleImage")))
			{
				TitleImage->SetVisibility(ESlateVisibility::Hidden);
			}
			if (UBorder* BackBoard = Cast<UBorder>(MenuWidgetInstance->GetWidgetFromName("BackBoard")))
			{
				BackBoard->SetVisibility(ESlateVisibility::Visible);
			}
			if (UTextBlock* TitleText = Cast<UTextBlock>(MenuWidgetInstance->GetWidgetFromName(TEXT("TitleText"))))
			{
				TitleText->SetText(FText::FromString(TEXT("스테이지 클리어")));
			}
			if (UTextBlock* SubTitleText = Cast<UTextBlock>(MenuWidgetInstance->GetWidgetFromName(TEXT("SubTitleText"))))
			{
				SubTitleText->SetVisibility(ESlateVisibility::Hidden);
			}
			if (UButton* StartButton = Cast<UButton>(MenuWidgetInstance->GetWidgetFromName("StartButton")))
			{
				StartButton->SetVisibility(ESlateVisibility::Hidden);
			}
			if (UTextBlock* ButtonText = Cast<UTextBlock>(MenuWidgetInstance->GetWidgetFromName(TEXT("ExitButton"))))
			{
				ButtonText->SetText(FText::FromString(TEXT("다음 스테이지")));
			}
			break;
		case 2:
			//게임 오버
			if (UImage* TitleImage = Cast<UImage>(MenuWidgetInstance->GetWidgetFromName("TitleImage")))
			{
				TitleImage->SetVisibility(ESlateVisibility::Hidden);
			}
			if (UBorder* BackBoard = Cast<UBorder>(MenuWidgetInstance->GetWidgetFromName("BackBoard")))
			{
				BackBoard->SetVisibility(ESlateVisibility::Visible);
			}
			if (UTextBlock* TitleText = Cast<UTextBlock>(MenuWidgetInstance->GetWidgetFromName(TEXT("TitleText"))))
			{
				TitleText->SetText(FText::FromString(TEXT("게임 오버")));
			}
			if (UTextBlock* ButtonText = Cast<UTextBlock>(MenuWidgetInstance->GetWidgetFromName(TEXT("StartButton"))))
			{
				ButtonText->SetText(FText::FromString(TEXT("다시 시작")));
			}
			if (UTextBlock* ButtonText = Cast<UTextBlock>(MenuWidgetInstance->GetWidgetFromName(TEXT("ExitButton"))))
			{
				ButtonText->SetText(FText::FromString(TEXT("메인 메뉴")));
			}
			break;
		default:
			break;
		}
	}
}

void ASanzoPlayerController::StartGame()
{
	if (USanzoGameInstance* SanzoGameInstance = Cast<USanzoGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		//게임 시작시 초기화 필요한 값 입력
	}
	//임시 코드, 이후 첫번째 스테이지 맵으로 변경 필요
	UGameplayStatics::OpenLevel(GetWorld(), FName("L_ThirdPerson"));
	SetPause(false);
}

void ASanzoPlayerController::ReturnMainMenu()
{
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
#pragma endregion
