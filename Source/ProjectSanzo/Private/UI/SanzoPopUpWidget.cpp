// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SanzoPopUpWidget.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void USanzoPopUpWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UpgradeButtons.Empty();
	UpgradeTexts.Empty();

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
	for (int32 i = 0; i < UpgradeButtons.Num(); i++)
	{
		SetUpgradeButton(i);
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

void USanzoPopUpWidget::SetUpgradeButton(int32 index)
{
	if (UpgradeButtons[index])
	{
		UpgradeButtons[index]->SetVisibility(ESlateVisibility::Visible);
		//임시 값
		int32 RandValue = FMath::RandRange(10, 100);
		UpgradeButtons[index]->SetBackgroundColor(GetColorByRarity(RandValue));
		if (UpgradeTexts[index])
		{
			//테스트 코드
			UpgradeTexts[index]->SetText(FText::FromString(FString::Printf(TEXT("공격력 + %d"), RandValue)));
		}
	}
}

//임시
FLinearColor USanzoPopUpWidget::GetColorByRarity(int32 Value)
{
	if (Value > 85)
	{
		return FLinearColor(1.0f, 0.72f, 0.0f);
	}
	if (Value > 50)
	{
		return FLinearColor(0.6f, 0.2f, 0.9f);
	}
	if (Value > 25)
	{
		return FLinearColor(0.0f, 0.4f, 0.9f);
	}
	return FLinearColor::White;
}
