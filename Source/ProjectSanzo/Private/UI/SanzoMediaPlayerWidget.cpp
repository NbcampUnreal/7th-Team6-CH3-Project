// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SanzoMediaPlayerWidget.h"
#include "MediaPlayer.h"
#include "MediaSoundComponent.h"
#include "MediaSource.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"


void USanzoMediaPlayerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetIsFocusable(true);

	if (CommonMediaPlayer)
	{
		CommonMediaPlayer->OnEndReached.AddDynamic(this, &USanzoMediaPlayerWidget::OnMediaEndReached);
	}

	// 사운드 컴포넌트 생성
	if (CommonMediaPlayer)
	{
		APlayerController* PlayerController = GetOwningPlayer();
		if (PlayerController)
		{
			MediaSoundComponent = NewObject<UMediaSoundComponent>(PlayerController);
			MediaSoundComponent->RegisterComponent();
			MediaSoundComponent->SetMediaPlayer(CommonMediaPlayer);
			MediaSoundComponent->SetVolumeMultiplier(1.0f);
		}
	}
	if (SkipText)
	{
		SkipText->SetText(FText::FromString(FString::Printf(TEXT("아무키나 입력하여 스킵하세요."))));
	}
	if (SkipTextAnim)
	{
		PlayAnimation(SkipTextAnim, 0.0f, 0);
	}
	
}


void USanzoMediaPlayerWidget::PlayMediaScene(FGameplayTag State)
{
	if (!CommonMediaPlayer || !MediaSourceArray.Contains(State)) return;
	
	if (UMediaSource* SelectedSource = MediaSourceArray[State].LoadSynchronous())
	{
		CommonMediaPlayer->OpenSource(SelectedSource);		
	}
	
	if (StoryTextOverlay)
	{
		if (State == FGameplayTag::RequestGameplayTag(FName("UI.State.Opening")))
		{
			StoryTextOverlay->SetVisibility(ESlateVisibility::Visible);
		}
		else if (State == FGameplayTag::RequestGameplayTag(FName("UI.State.Ending")))
		{
			StoryTextOverlay->SetVisibility(ESlateVisibility::Visible);
		}
		else if (State == FGameplayTag::RequestGameplayTag(FName("UI.State.TrueEnding")))
		{
			StoryTextOverlay->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

FReply USanzoMediaPlayerWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FinishedMediaScene();
	
	return FReply::Handled();
}

FReply USanzoMediaPlayerWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FinishedMediaScene();
	
	return FReply::Handled();
}

void USanzoMediaPlayerWidget::OnMediaEndReached()
{
	FinishedMediaScene();
}

void USanzoMediaPlayerWidget::FinishedMediaScene()
{
	if (CommonMediaPlayer)
	{
		CommonMediaPlayer->Close();
	}
	
	if (OnMediaSceneFinished.IsBound())
	{
		OnMediaSceneFinished.Broadcast();
	}
	
	RemoveFromParent();
	
}

void USanzoMediaPlayerWidget::NativeDestruct()
{
	if (CommonMediaPlayer)
	{
		CommonMediaPlayer->OnEndReached.RemoveDynamic(this, &USanzoMediaPlayerWidget::OnMediaEndReached);
	}
	
	if (MediaSoundComponent)
	{
		MediaSoundComponent->Stop();
		MediaSoundComponent->DestroyComponent();
		MediaSoundComponent = nullptr;
	}
	
	Super::NativeDestruct();
}