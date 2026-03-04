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
	
	if (MediaSkipProgressBar)
	{
		SkipProgressBarDynamic = MediaSkipProgressBar->GetDynamicMaterial();
		UpdateSkipProgress(0);
	}
}


void USanzoMediaPlayerWidget::PlayMediaScene(FGameplayTag State)
{


	if (!CommonMediaPlayer || !MediaSourceArray.Contains(State))
	{
		return;
	}

	CurrentPlayingTag = State;

	if (UMediaSource* SelectedSource = MediaSourceArray[State].LoadSynchronous())
	{
		CommonMediaPlayer->OpenSource(SelectedSource);
	}

	if (StoryTextOverlay)
	{
		if (State == FGameplayTag::RequestGameplayTag(FName("UI.State.Opening")))
		{
		  StoryTextOverlay->SetVisibility(ESlateVisibility::Visible);
		  
		  if (DisappearTextBoxAnim)
		  {
		    PlayAnimation(DisappearTextBoxAnim);
		  }
		}
		else if (State == FGameplayTag::RequestGameplayTag(FName("UI.State.Ending")))
		{
			StoryTextOverlay->SetVisibility(ESlateVisibility::Visible);
		  if (DisappearEndingTextBoxAnim)
		  {
		    PlayAnimation(DisappearEndingTextBoxAnim);
		  }
		}
		else if (State == FGameplayTag::RequestGameplayTag(FName("UI.State.TrueEnding")))
		{
			StoryTextOverlay->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	CurrentSubtitleIndex = 0;
	if (StoryText)
	{
		StoryText->SetText(FText::GetEmpty());
	}
	GetWorld()->GetTimerManager().SetTimer(
		SubtitlesTimerHandle,
		this,
		&USanzoMediaPlayerWidget::UpdateSubtitle,
		0.1f,
		true
	);
}

void USanzoMediaPlayerWidget::OnMediaEndReached()
{
  GetWorld()->GetTimerManager().ClearTimer(SubtitlesTimerHandle);
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


#pragma region Subtitles


void USanzoMediaPlayerWidget::UpdateSubtitle()
{
	if (!CommonMediaPlayer || !CommonMediaPlayer->IsPlaying())
	{
		return;
	}

	FMediaSubTitleList* SubTitleList = SubtitleData.Find(CurrentPlayingTag);
	if (!SubTitleList || !SubTitleList->Subtitles.IsValidIndex(CurrentSubtitleIndex))
	{
		return;
	}

	float CurrentTime = CommonMediaPlayer->GetTime().GetTotalSeconds();
	if (CurrentTime >= SubTitleList->Subtitles[CurrentSubtitleIndex].TimeStamp)
	{
		if (StoryText)
		{
			StoryText->SetText(SubTitleList->Subtitles[CurrentSubtitleIndex].SubtitleText);
		}
		CurrentSubtitleIndex++;
	}
}

#pragma endregion 이준로

#pragma region Skip

void USanzoMediaPlayerWidget::NativeTick(const FGeometry& MyGeometry, float InDT)
{
	Super::NativeTick(MyGeometry, InDT);

	if (IsAnyKeyPressed)
	{
		float CurrentDuration = GetWorld()->GetTimeSeconds() - PressStartTime;
		float CurrentPercent = FMath::Clamp(CurrentDuration / TimeToSkip, 0.0f, 1.0f);

		UpdateSkipProgress(CurrentPercent);
	}
}

FReply USanzoMediaPlayerWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (!GetWorld()->GetTimerManager().IsTimerActive(SkipTimerHandle))
	{
		PressStartTime = GetWorld()->GetTimeSeconds();
		IsAnyKeyPressed = true;
		GetWorld()->GetTimerManager().SetTimer(
			SkipTimerHandle,
			this,
			&USanzoMediaPlayerWidget::ExecuteSkip,
			TimeToSkip,
			false
		);
	}

	return FReply::Handled();
}

FReply USanzoMediaPlayerWidget::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (GetWorld()->GetTimerManager().IsTimerActive(SkipTimerHandle))
	{
		IsAnyKeyPressed = false;
		UpdateSkipProgress(0);
		GetWorld()->GetTimerManager().ClearTimer(SkipTimerHandle);
	}

	return FReply::Handled();
}

FReply USanzoMediaPlayerWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!GetWorld()->GetTimerManager().IsTimerActive(SkipTimerHandle))
	{
		PressStartTime = GetWorld()->GetTimeSeconds();
		IsAnyKeyPressed = true;
		GetWorld()->GetTimerManager().SetTimer(
			SkipTimerHandle,
			this,
			&USanzoMediaPlayerWidget::ExecuteSkip,
			TimeToSkip,
			false
		);
	}
	return FReply::Handled();
}

FReply USanzoMediaPlayerWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (GetWorld()->GetTimerManager().IsTimerActive(SkipTimerHandle))
	{
		IsAnyKeyPressed = false;
		UpdateSkipProgress(0);
		GetWorld()->GetTimerManager().ClearTimer(SkipTimerHandle);
	}

	return FReply::Handled();
}

void USanzoMediaPlayerWidget::UpdateSkipProgress(float NewPercent)
{
	if (SkipProgressBarDynamic)
	{
		SkipProgressBarDynamic->SetScalarParameterValue(TEXT("Percentage"), NewPercent);
	}

	if (MediaSkipProgressBar)
	{
		bool bIsVisible = NewPercent > 0.0f;
		ESlateVisibility SkipProgressBarVisibility = bIsVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
		MediaSkipProgressBar->SetVisibility(SkipProgressBarVisibility);

		if (bIsVisible)
		{
			FLinearColor StartColor = FLinearColor(0.3f, 0.3f, 0.3f);
			FLinearColor EndColor = FLinearColor::White;

			// NewPercent(0~1)에 따라 두 색상 사이의 값을 계산
			FLinearColor CurrentColor = FLinearColor::LerpUsingHSV(StartColor, EndColor, NewPercent);

			// 위젯의 색조(Tint)를 설정
			MediaSkipProgressBar->SetColorAndOpacity(CurrentColor);
		}
	}
}

void USanzoMediaPlayerWidget::ExecuteSkip()
{
	FinishedMediaScene();
}

#pragma endregion 이준로