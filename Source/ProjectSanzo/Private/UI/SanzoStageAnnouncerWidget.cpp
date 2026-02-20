// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SanzoStageAnnouncerWidget.h"

#include "GameplayTagContainer.h"
#include "Components/TextBlock.h"

void USanzoStageAnnouncerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (AnnounceTextAnim)
	{
		PlayAnimation(AnnounceTextAnim);
		
	}
	
	if (SubAnnounceTextAnim)
	{
		FWidgetAnimationDynamicEvent EndEvent;
		EndEvent.BindDynamic(this, &USanzoStageAnnouncerWidget::OnAnimationFinishedHandle);
		
		BindToAnimationFinished(SubAnnounceTextAnim, EndEvent);
		
		PlayAnimation(SubAnnounceTextAnim);
	}
	
}

void USanzoStageAnnouncerWidget::SetStageAnnouncer(FGameplayTag State, ESanzoStageType StageType)
{
	if (State == FGameplayTag::RequestGameplayTag(FName("Room.State.Actived")))
	{
		SetStageStartUI(StageType);
	}
	if (State == FGameplayTag::RequestGameplayTag(FName("Room.State.Cleared")))
	{
		SetStageClearUI();
	}
}

void USanzoStageAnnouncerWidget::SetStageStartUI(ESanzoStageType StageType)
{
	if (StageType == ESanzoStageType::Extermination)
	{
		if (AnnounceText)
		{
			AnnounceText->SetText(FText::FromString(FString::Printf(TEXT("섬멸전"))));
		}
		if (SubAnnounceText)
		{
			SubAnnounceText->SetText(FText::FromString(FString::Printf(TEXT("모든 적을 섬멸하라!"))));
		}
	}
	
	if (StageType == ESanzoStageType::Survival)
	{
		if (AnnounceText)
		{
			AnnounceText->SetText(FText::FromString(FString::Printf(TEXT("방호전"))));
		}
		if (SubAnnounceText)
		{
			SubAnnounceText->SetText(FText::FromString(FString::Printf(TEXT("주어진 시간동안 방어하라!"))));
		}
	}
	
	if (StageType == ESanzoStageType::Boss)
	{
		if (AnnounceText)
		{
			AnnounceText->SetText(FText::FromString(FString::Printf(TEXT("대장전"))));
		}
		if (SubAnnounceText)
		{
			SubAnnounceText->SetText(FText::FromString(FString::Printf(TEXT("적 대장을 처치하라!"))));
		}
	}
}

void USanzoStageAnnouncerWidget::SetStageClearUI()
{
	if (AnnounceText)
	{
		AnnounceText->SetText(FText::FromString(FString::Printf(TEXT("목표 완료"))));
	}
	if (SubAnnounceText)
	{
		SubAnnounceText->SetText(FText::FromString(FString::Printf(TEXT("문을 찾아 이동하세요."))));
	}
}

void USanzoStageAnnouncerWidget::OnAnimationFinishedHandle()
{
	OnAnnouncerEnded.Broadcast();
}
