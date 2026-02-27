// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SanzoStageInfoWidget.h"

#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Core/SanzoGameState.h"

void USanzoStageInfoWidget::SetStageInfo(ESanzoStageType StageType)
{
	if (StageType == ESanzoStageType::Boss)
	{
		SetBossStageInfo();
	}
	else
	{
		SetDefaultStageInfo();
	}
}

void USanzoStageInfoWidget::SetDefaultStageInfo()
{
	if (ASanzoGameState* GameState = GetWorld()->GetGameState<ASanzoGameState>())
	{
		if (StageInfoText)
		{
			int32 StageInfo = GameState->CurrentStageIndex;
			StageInfoText->SetText(FText::FromString(FString::Printf(TEXT("스테이지: %d"), StageInfo)));
		}
		//Delegate 등록
		//this : 대상 객체, &USanzoHUDWidget::HandleStageProgressChanged: 실행할 함수
		GameState->OnStageProgressChanged.AddDynamic(this, &USanzoStageInfoWidget::HandleStageProgressChanged);
	}
}

void USanzoStageInfoWidget::SetBossStageInfo()
{
	if (StageInfoText)
	{
		FSlateFontInfo NewFontInfo = StageInfoText->GetFont();
		NewFontInfo.Size = 60;
		
		StageInfoText->SetFont(NewFontInfo);
		
		StageInfoText->SetText(FText::FromString(FString::Printf(TEXT("캬라신 케이하쿠"))));
	}
	
	if (StageProgressBarSizeBox)
	{
		StageProgressBarSizeBox->SetWidthOverride(1300);
		StageProgressBarSizeBox->SetHeightOverride(30);
	}
	
	if (StunGageBox)
	{
		StunImages.Empty();
		
		for (int i = 0; i < StunGageBox->GetChildrenCount(); i++)
		{
			if (UImage* ChildImage = Cast<UImage>(StunGageBox->GetChildAt(i)))
			{
				StunImages.Add(ChildImage);
			}
		}
		
		StunGageBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	
	//보스 연결 필요
}

void USanzoStageInfoWidget::HandleStageProgressChanged(float Percent)
{
	StageProgressBar->SetPercent(Percent);
}

void USanzoStageInfoWidget::HandleBossHealthChanged(float Percent)
{
	StageProgressBar->SetPercent(Percent);
}

FString USanzoStageInfoWidget::GetRandomBossName()
{
	int32 RandValue = FMath::RandRange(0, 4);
	if (RandValue == 0)
	{
		return "캬라신 케이하쿠";
	}
	if (RandValue == 1)
	{
		return "에네미치 도쥬";
	}
	if (RandValue == 2)
	{
		return "";
	}
	if (RandValue == 3)
	{
		return "";
	}
	if (RandValue == 4)
	{
		return "";
	}
	return "";
}

