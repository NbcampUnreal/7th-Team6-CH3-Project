// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/SanzoPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "UI/SanzoMainWidget.h"
#include "UI/SanzoHUDWidget.h"
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
	FString CurrentMapName = GetWorld()->GetMapName();
	if (CurrentMapName.Contains("L_MainMenu"))
	{
		ShowMainUI(0);
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

void ASanzoPlayerController::ShowPopUp(bool bIsLevelUp)
{
	if (PopUpWidgetInstance)
	{
		return;
	}
	if (PopUpWidgetClass)
	{
		PopUpWidgetInstance = CreateWidget<UUserWidget>(this, PopUpWidgetClass);
		{
			if (PopUpWidgetInstance)
			{
				PopUpWidgetInstance->AddToViewport();

				SetPause(true);
				bShowMouseCursor = true;
				SetInputMode(FInputModeUIOnly());

				//캐릭터 Status 출력
				//나중에 Instance에서 정보를 받아올 경우 멤버변수로 선언하여 데이터를 저장하는 방식으로 변경 필요해보임
				if (UTextBlock* StatusText = Cast<UTextBlock>(PopUpWidgetInstance->GetWidgetFromName("StatusText")))
				{
					FText StatusTemplate = FText::FromString(TEXT(
							"체력: {HP} (+ {AddHP})\n"
							"스태미나: {Stamina} (+ {AddStamina})\n"
							"공격력: {Atk} (+ {AddAtk})\n"
							"공격 속도: {AtkSpd} (+ {AddAtkSpd})\n"
							"이동 속도: {MoveSpd} (+ {AddMoveSpd})"
						)
					);

					FFormatNamedArguments Arguments;
					Arguments.Add(TEXT("HP"), 200);
					Arguments.Add(TEXT("AddHP"), 50);
					Arguments.Add(TEXT("Stamina"), 100);
					Arguments.Add(TEXT("AddStamina"), 20);
					Arguments.Add(TEXT("Atk"), 20);
					Arguments.Add(TEXT("AddAtk"), 10);
					Arguments.Add(TEXT("AtkSpd"), 1000);
					Arguments.Add(TEXT("AddAtkSpd"), 200);
					Arguments.Add(TEXT("MoveSpd"), 300);
					Arguments.Add(TEXT("AddMoveSpd"), 40);

					StatusText->SetText(FText::Format(StatusTemplate, Arguments));
				}
				if (bIsLevelUp)
				{
					if (UTextBlock* PopUpText = Cast<UTextBlock>(PopUpWidgetInstance->GetWidgetFromName(TEXT("PopUpText"))))
					{
						PopUpText->SetText(FText::FromString(TEXT("레벨 업!")));
					}
					if (UBorder* UpgradeListBorder = Cast<UBorder>(PopUpWidgetInstance->GetWidgetFromName("UpgradeListBorder")))
					{
						UpgradeListBorder->SetVisibility(ESlateVisibility::Hidden);
					}
					if (UButton* UpgradeButton_1 = Cast<UButton>(PopUpWidgetInstance->GetWidgetFromName("UpgradeButton_1")))
					{
						UE_LOG(LogTemp, Warning, TEXT("Find Button_1!"));
						UpgradeButton_1->SetVisibility(ESlateVisibility::Visible);
						//등급에 따른 카드 색 변화, 업그레이드 정보에서 가져올 필요 있음
						UpgradeButton_1->SetBackgroundColor(FColor(128, 128, 128).ReinterpretAsLinear());

						if (UTextBlock* UpgradeText_1 = Cast<UTextBlock>(PopUpWidgetInstance->GetWidgetFromName("UpgradeText_1")))
						{
							//업그레이드 리스트로부터 정보 가져오는 함수 또는 동작 필요
							UpgradeText_1->SetText(FText::FromString(TEXT("체력 + 100")));
						}
					}
					if (UButton* UpgradeButton_2 = Cast<UButton>(PopUpWidgetInstance->GetWidgetFromName("UpgradeButton_2")))
					{
						UE_LOG(LogTemp, Warning, TEXT("Find Button_2!"));
						UpgradeButton_2->SetVisibility(ESlateVisibility::Visible);
						//등급에 따른 카드 색 변화, 업그레이드 정보에서 가져올 필요 있음
						UpgradeButton_2->SetBackgroundColor(FColor(0, 112, 221).ReinterpretAsLinear());

						if (UTextBlock* UpgradeText_2 = Cast<UTextBlock>(PopUpWidgetInstance->GetWidgetFromName("UpgradeText_2")))
						{
							//업그레이드 리스트로부터 정보 가져오는 함수 또는 동작 필요
							UpgradeText_2->SetText(FText::FromString(TEXT("체력 + 200")));
						}
					}
					if (UButton* UpgradeButton_3 = Cast<UButton>(PopUpWidgetInstance->GetWidgetFromName("UpgradeButton_3")))
					{
						UE_LOG(LogTemp, Warning, TEXT("Find Button_3!"));
						UpgradeButton_3->SetVisibility(ESlateVisibility::Visible);
						//등급에 따른 카드 색 변화, 업그레이드 정보에서 가져올 필요 있음
						UpgradeButton_3->SetBackgroundColor(FColor(163, 53, 238).ReinterpretAsLinear());

						if (UTextBlock* UpgradeText_3 = Cast<UTextBlock>(PopUpWidgetInstance->GetWidgetFromName("UpgradeText_3")))
						{
							//업그레이드 리스트로부터 정보 가져오는 함수 또는 동작 필요
							UpgradeText_3->SetText(FText::FromString(TEXT("체력 + 300")));
						}
					}
					if (UButton* MainMenuButton = Cast<UButton>(PopUpWidgetInstance->GetWidgetFromName("MainMenuButton")))
					{
						MainMenuButton->SetVisibility(ESlateVisibility::Hidden);
					}
					if (UButton* ResumeButton = Cast<UButton>(PopUpWidgetInstance->GetWidgetFromName("ResumeButton")))
					{
						ResumeButton->SetVisibility(ESlateVisibility::Hidden);
					}
				}
				else
				{
					if (UTextBlock* PopUpText = Cast<UTextBlock>(PopUpWidgetInstance->GetWidgetFromName(TEXT("PopUpText"))))
					{
						PopUpText->SetText(FText::FromString(TEXT("일시 정지")));
					}
					//강화 목록 업데이트 관련 구현 필요.
				}
			}
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

void ASanzoPlayerController::ShowMainUI(int32 CaseIndex)
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (MenuWidgetInstance)
	{
		MenuWidgetInstance->Super::RemoveFromParent();
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
		switch (CaseIndex)
		{
		case 0:
			//메인 메뉴
			MenuWidgetInstance->SetMainMenuUI();
			break;
		case 1:
			//스테이지 클리어
			//이후 StageManger 또는 GameState에서 정보 받아서 구현 필요.
			MenuWidgetInstance->SetStageClearMenuUI(10.23f, 30);
			break;
		case 2:
			//게임 오버
			MenuWidgetInstance->SetGameOverMenuUI();
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
