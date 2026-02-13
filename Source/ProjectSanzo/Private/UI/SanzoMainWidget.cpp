// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SanzoMainWidget.h"

#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void USanzoMainWidget::SetMainMenuUI()
{
	if (BackBoard)
	{
		BackBoard->SetVisibility(ESlateVisibility::Hidden);
	}
	if (StartText)
	{
		StartText->SetText(FText::FromString(TEXT("게임 시작")));
	}
	if (ExitText)
	{
		ExitText->SetText(FText::FromString(TEXT("게임 종료")));
	}
}

void USanzoMainWidget::SetStageClearMenuUI(float ClearTime, int32 KillScore)
{
	if (TitleImage)
	{
		TitleImage->SetVisibility(ESlateVisibility::Hidden);
	}
	if (SubTitleText)
	{
		SubTitleText->SetVisibility(ESlateVisibility::Hidden);
	}
	if (StartButton)
	{
		StartButton->SetVisibility(ESlateVisibility::Hidden);
	}
	if (ClearTimeText)
	{
		ClearTimeText->SetVisibility(ESlateVisibility::Visible);
		ClearTimeText->SetText(FText::FromString(FString::Printf(TEXT("클리어 시간: %f"),ClearTime)));
	}
	if (KillScoreText)
	{
		KillScoreText->SetVisibility(ESlateVisibility::Visible);
		KillScoreText->SetText(FText::FromString((FString::Printf(TEXT("적 처치 수: %d"), KillScore))));
	}
	if (TitleText)
	{
		TitleText->SetText(FText::FromString(TEXT("스테이지 클리어")));
	}
	if (ExitText)
	{
		ExitText->SetText(FText::FromString(TEXT("다음 스테이지")));
	}
	
}

void USanzoMainWidget::SetGameOverMenuUI()
{
	if (TitleImage)
	{
		TitleImage->SetVisibility(ESlateVisibility::Hidden);
	}
	if (SubTitleText)
	{
		SubTitleText->SetVisibility(ESlateVisibility::Hidden);
	}
	if (TitleText)
	{
		TitleText->SetText(FText::FromString(TEXT("게임 오버")));
	}
	if (StartText)
	{
		StartText->SetText(FText::FromString(TEXT("다시 시작")));
	}
	if (ExitText)
	{
		ExitText->SetText(FText::FromString(TEXT("메인 메뉴")));
	}
}
