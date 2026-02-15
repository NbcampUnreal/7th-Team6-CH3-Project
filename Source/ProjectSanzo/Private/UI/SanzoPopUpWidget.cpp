// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SanzoPopUpWidget.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Core/UpgradeSystem/SanzoUpgradeSubsystem.h"

void USanzoPopUpWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UpgradeButtons.Empty();
	UpgradeTexts.Empty();

	if (UpgradeButton_1)
	{
		UpgradeButtons.Add(UpgradeButton_1);
		UpgradeButton_1->OnClicked.AddDynamic(this,&USanzoPopUpWidget::OnUpgradeButton1Clicked);
	}
	if (UpgradeButton_2)
	{
		UpgradeButtons.Add(UpgradeButton_2);
		UpgradeButton_2->OnClicked.AddDynamic(this,&USanzoPopUpWidget::OnUpgradeButton2Clicked);
	}
	if (UpgradeButton_3)
	{
		UpgradeButtons.Add(UpgradeButton_3);
		UpgradeButton_1->OnClicked.AddDynamic(this,&USanzoPopUpWidget::OnUpgradeButton3Clicked);
	}

	if (UpgradeText_1)
	{
		UpgradeTexts.Add(UpgradeText_1);
	}
	if (UpgradeText_2)
	{
		UpgradeTexts.Add(UpgradeText_2);
	}
	if (UpgradeText_3)
	{
		UpgradeTexts.Add(UpgradeText_3);
	}
}

void USanzoPopUpWidget::OnUpgradeButton1Clicked() { ProcessUpgradeButtonClicked(0); }
void USanzoPopUpWidget::OnUpgradeButton2Clicked() { ProcessUpgradeButtonClicked(1); }
void USanzoPopUpWidget::OnUpgradeButton3Clicked() { ProcessUpgradeButtonClicked(2); } 

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
			SetUpgradeListText();
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
	
	USanzoUpgradeSubsystem* UpgradeSubsystem = GetGameInstance()->GetSubsystem<USanzoUpgradeSubsystem>();
	if (UpgradeSubsystem)
	{
		CurrentOptions = UpgradeSubsystem->GeneratedRandomOptions();
		for (int32 i = 0; i < UpgradeButtons.Num(); i++)
		{
			SetUpgradeButton(i, CurrentOptions[i]);
		}
	}
}

void USanzoPopUpWidget::ProcessUpgradeButtonClicked(int32 Index)
{
	if (!CurrentOptions.IsValidIndex(Index)) return;
	
	FUpgradeOption SelectedOption = CurrentOptions[Index];
	
	USanzoUpgradeSubsystem* UpgradeSubsystem = GetGameInstance()->GetSubsystem<USanzoUpgradeSubsystem>();
	if (UpgradeSubsystem)
	{
		UpgradeSubsystem->ProcessSelectedUpgrade(SelectedOption);
	}
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

void USanzoPopUpWidget::SetUpgradeListText()
{
	if (UpgradeListText)
	{
		UpgradeListText->SetText(FText::FromString(TEXT("강화 목록")));
	}
}

void USanzoPopUpWidget::SetUpgradeButton(int32 index, const FUpgradeOption& option)
{
	if (UpgradeButtons.IsValidIndex(index) && UpgradeButtons[index])
	{
		UpgradeButtons[index]->SetVisibility(ESlateVisibility::Visible);
		UpgradeButtons[index]->SetBackgroundColor(GetColorByRarity(option.Rarity));
		
		if (UpgradeTexts.IsValidIndex(index) && UpgradeTexts[index])
		{
			FText DisplayText = FText::Format(FText::FromString("{0} + {1}"),
				option.DisplayName, FText::AsNumber(option.Value)
				);
			UpgradeTexts[index]->SetText(DisplayText);
		}
	}
}

FLinearColor USanzoPopUpWidget::GetColorByRarity(EUpgradeRarity Rarity)
{
	switch (Rarity)
	{
	case EUpgradeRarity::Legend:
		return FLinearColor(1.0f, 0.72f, 0.0f);

	case EUpgradeRarity::Epic:
		return FLinearColor(0.6f, 0.2f, 0.9f);

	case EUpgradeRarity::Rare:
		return FLinearColor(0.0f, 0.4f, 0.9f);

	case EUpgradeRarity::Common:
		return FLinearColor(0.5f,0.5f,0.5f);
		
	default:
		return FLinearColor::Black;
	}
}
