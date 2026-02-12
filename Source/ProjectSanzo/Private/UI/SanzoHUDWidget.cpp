// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SanzoHUDWidget.h"

#include "Character/Components/SanzoStatComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

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

void USanzoHUDWidget::UpdateStageProgressBar()
{
}
