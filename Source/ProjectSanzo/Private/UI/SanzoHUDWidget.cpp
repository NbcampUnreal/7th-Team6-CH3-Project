// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SanzoHUDWidget.h"

#include "Character/Components/SanzoStatComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Core/SanzoGameState.h"

void USanzoHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (APawn* PlayerCharacter = GetOwningPlayerPawn())
	{
		//StatComponent 추출
		USanzoStatComponent* StatComponent = PlayerCharacter->FindComponentByClass<USanzoStatComponent>();
		if (StatComponent)
		{
			//Delegate 등록
			//this : 대상 객체, &USanzoHUDWidget::HandleStatChanged: 실행할 함수
			StatComponent->OnStatChanged.AddDynamic(this, &USanzoHUDWidget::HandleStatChanged);
		}
	}

	if (ASanzoGameState* GameState = GetWorld()->GetGameState<ASanzoGameState>())
	{
		if (StageText)
		{
			int32 StageInfo = GameState->CurrentStageIndex + 1;
			StageText->SetText(FText::FromString(FString::Printf(TEXT("스테이지: %d"), StageInfo)));
		}
		//Delegate 등록
		//this : 대상 객체, &USanzoHUDWidget::HandleStageProgressChanged: 실행할 함수
		GameState->OnStageProgressChanged.AddDynamic(this, &USanzoHUDWidget::HandleStageProgressChanged);
	}
}

void USanzoHUDWidget::HandleStatChanged(const FSanzoStatData& Data)
{
	if (HealthBar)
	{
		HealthBar->SetPercent(Data.HealthPercent);
	}
	if (StaminaBar)
	{
		StaminaBar->SetPercent(Data.StaminaPercent);
	}
	if (ExpBar)
	{
		ExpBar->SetPercent(Data.ExpPercent);
	}
	if (LevelText)
	{
		LevelText->SetText(FText::AsNumber(Data.CurrentLevel));
	}
}

void USanzoHUDWidget::HandleStageProgressChanged(float percent)
{
	if (StageProgressBar)
	{
		StageProgressBar->SetPercent(percent);
	}
}
