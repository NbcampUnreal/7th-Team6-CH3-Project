// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SanzoPopUpWidget.h"

#include "Common/SanzoLog.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/WrapBox.h"
#include "Core/UpgradeSystem/SanzoUpgradeSubsystem.h"
#include "UI/SanzoUpgradeButtonWidget.h"
#include "UI/SanzoUpgradeInfo.h"

void USanzoPopUpWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	UpgradeButtons.Empty();
	GamePlayingTag = FGameplayTag::RequestGameplayTag(FName("Game.State.Playing"));
	MainMenuTag = FGameplayTag::RequestGameplayTag(FName("Game.State.MainMenu"));
	
	if (ResumeButton)
	{
		ResumeButton->OnClicked.AddDynamic(this,&ThisClass::HandleResumeButtonClicked);
	}
	
	if (MainMenuButton)
	{
		MainMenuButton->OnClicked.AddDynamic(this, &ThisClass::HandleMenuButtonClicked);
	}

	if (UpgradeButton_1)
	{
		UpgradeButtons.Add(UpgradeButton_1);
	}
	if (UpgradeButton_2)
	{
		UpgradeButtons.Add(UpgradeButton_2);
	}
	if (UpgradeButton_3)
	{
		UpgradeButtons.Add(UpgradeButton_3);
	}
	for (auto* UpgradeButton : UpgradeButtons)
	{
		UpgradeButton->OnUpgradeButtonClicked.AddDynamic(this,&ThisClass::HandleUpgradeSelected);
	}
}

void USanzoPopUpWidget::SetPopUpUI(FGameplayTag GameState)
{
	CurrentState = GameState;
	
	if (GameState == FGameplayTag::RequestGameplayTag(FName("Game.State.Paused")))
	{
		SetPauseUI();
	}
	if (GameState == FGameplayTag::RequestGameplayTag(FName("Game.State.UpgradeSelecting")))
	{
		SetUpgradeUI();
	}
}

void USanzoPopUpWidget::SetPauseUI()
{
	if (PopUpText)
	{
		PopUpText->SetText(FText::FromString(TEXT("일시 정지")));
	}
	if (StatusText)
	{
		SetStatusText();
	}
	if (MainMenuButton)
	{
		MainMenuButton->SetVisibility(ESlateVisibility::Visible);
		if (MainMenuText)
		{
			MainMenuText->SetText(FText::FromString(TEXT("메인 메뉴")));
		}
	}
	if (ResumeButton)
	{
		ResumeButton->SetVisibility(ESlateVisibility::Visible);
		if (ResumeText)
		{
			ResumeText->SetText(FText::FromString(TEXT("계속")));
		}
	}

	if (UpgradeListBorder)
	{
		UpgradeListBorder->SetVisibility(ESlateVisibility::Visible);
		if (UpgradeListText)
		{
			SetUpgradeList();
		}
	}
}

void USanzoPopUpWidget::SetUpgradeUI()
{
	if (PopUpText)
	{
		PopUpText->SetText(FText::FromString(TEXT("레벨 업")));
	}
	if (StatusText)
	{
		SetStatusText();
	}
	if (MainMenuButton)
	{
		MainMenuButton->SetVisibility(ESlateVisibility::Hidden);
	}
	if (ResumeButton)
	{
		ResumeButton->SetVisibility(ESlateVisibility::Hidden);
	}
	if (UpgradeListBorder)
	{
		UpgradeListBorder->SetVisibility(ESlateVisibility::Hidden);
	}
	
	if (UpgradeButtonBox)
	{
		UpgradeButtonBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	
	USanzoUpgradeSubsystem* UpgradeSubsystem = GetGameInstance()->GetSubsystem<USanzoUpgradeSubsystem>();
	if (UpgradeSubsystem)
	{
		CurrentOptions = UpgradeSubsystem->GeneratedRandomOptions();
		for (int32 i = 0; i < UpgradeButtons.Num(); i++)
		{
			if (CurrentOptions.IsValidIndex(i))
			{
				SetUpgradeButton(i, CurrentOptions[i]);
			}
		}
	}
}

void USanzoPopUpWidget::HandleUpgradeSelected(const FUpgradeOption& SelectedUpgrade)
{
	USanzoUpgradeSubsystem* UpgradeSubsystem = GetGameInstance()->GetSubsystem<USanzoUpgradeSubsystem>();
	if (UpgradeSubsystem)
	{
		UpgradeSubsystem->ProcessSelectedUpgrade(SelectedUpgrade);
	}
	
	OnButtonClicked.Broadcast(GamePlayingTag);	
}

void USanzoPopUpWidget::HandleMenuButtonClicked()
{
	OnButtonClicked.Broadcast(MainMenuTag);
}

void USanzoPopUpWidget::HandleResumeButtonClicked()
{
	OnButtonClicked.Broadcast(GamePlayingTag);
}

void USanzoPopUpWidget::SetStatusText()
{
	if (StatusText)
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
}

void USanzoPopUpWidget::SetUpgradeList()
{
	if (UpgradeListText)
	{
		UpgradeListText->SetText(FText::FromString(TEXT("강화 목록")));
	}
	
	if (!UpgradeListWrapBox || !UpgradeInfoWidgetClass) return;
	
	UpgradeListWrapBox->ClearChildren();
	
	USanzoUpgradeSubsystem* Subsystem = GetGameInstance()->GetSubsystem<USanzoUpgradeSubsystem>();
	if (!Subsystem) return;
	
	const TMap<FName, int32>& SelectedTotalMap = Subsystem->GetSelectedTotalMap();
	
	for (const auto& Elem : SelectedTotalMap)
	{
		FName UpgradeID = Elem.Key;
		int32 Count = Elem.Value;
		
		UE_LOG(LogLJR, Log, TEXT("Processing Upgrade: %s (Count: %d)"), *UpgradeID.ToString(), Count);
		
		UUserWidget* UpgradeInfoWidget = CreateWidget<UUserWidget>(this, UpgradeInfoWidgetClass);
		if (UpgradeInfoWidget)
		{
			if (USanzoUpgradeInfo* InfoWidget = Cast<USanzoUpgradeInfo>(UpgradeInfoWidget))
			{
				InfoWidget->SetUpgradeInfoBlock(UpgradeID, Count);
			}
			UpgradeListWrapBox->AddChild(UpgradeInfoWidget);
		}
	}
}

void USanzoPopUpWidget::SetUpgradeButton(int32 Index, const FUpgradeOption& Option)
{
	if (UpgradeButtons.IsValidIndex(Index))
	{
		UpgradeButtons[Index]->SetUpgradeButton(Option);
	}
}
