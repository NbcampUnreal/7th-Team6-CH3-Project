// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SanzoMainWidget.h"

#include "Components/AudioComponent.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void USanzoMainWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	StartGameTag = FGameplayTag::RequestGameplayTag(FName("Game.State.Playing"));
	QuitGameTag = FGameplayTag::RequestGameplayTag(FName("Game.State.Quit"));
	NextStageTag = FGameplayTag::RequestGameplayTag(FName("Room.State.MoveNext"));
	ReturnMainMenuTag = FGameplayTag::RequestGameplayTag(FName("Game.State.MainMenu"));
	
	if (StartButton)
	{
		StartButton->OnClicked.AddDynamic(this,&ThisClass::HandleStartButtonClicked);
	}
	if (ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(this,&ThisClass::HandleExitButtonClicked);
	}
	
}

void USanzoMainWidget::NativeDestruct()
{
	if (PlayingAudio && PlayingAudio->IsPlaying())
	{
		PlayingAudio->Stop();
	}
	
	Super::NativeDestruct();
}

void USanzoMainWidget::SetMainUI(FGameplayTag State, float ClearTime, int32 KillScore)
{
	CurrentState = State;
	
	if (State == FGameplayTag::RequestGameplayTag(FName("Game.State.MainMenu")))
	{
		SetMainMenuUI();
	}
	//스테이지 클리어서 넘어가는 태그 확인 필요
	if (State == FGameplayTag::RequestGameplayTag(FName("Room.State.Cleared")))
	{
		SetStageClearMenuUI(ClearTime, KillScore);
	}
	if (State == FGameplayTag::RequestGameplayTag(FName("Game.State.GameOver")))
	{
		SetGameOverMenuUI();
	}
}

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
	if (StartButtonOverlay)
	{
		StartButtonOverlay->SetVisibility(ESlateVisibility::Hidden);
	}
	if (ClearTimeText)
	{
		//전체 분, 초 계산
		int32 TotalSeconds = FMath::FloorToInt(ClearTime);
		int32 Minutes = TotalSeconds / 60;
		int32 Seconds = TotalSeconds % 60;
		
		//밀리 초 계산
		int32 MilliSeconds = FMath::FloorToInt((ClearTime- TotalSeconds) * 100);
		
		FString TimeText = FString::Printf(TEXT("%02d : %02d . %02d"), Minutes, Seconds, MilliSeconds);
		
		ClearTimeText->SetVisibility(ESlateVisibility::Visible);
		ClearTimeText->SetText(FText::Format(FText::FromString(TEXT("클리어 시간: {0}")), FText::FromString(TimeText)));
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
	
	if (StageClearAnim)
	{
		PlayAnimation(StageClearAnim);
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
	
	PlayGameOverSound();
	
	if (GameOverAnim)
	{
		PlayAnimation(GameOverAnim);
	}
	
}

void USanzoMainWidget::HandleStartButtonClicked()
{
	if (StartSound)
	{
		UAudioComponent* ClickAudioComponent = UGameplayStatics::SpawnSound2D(this, StartSound);

		if (ClickAudioComponent)
		{
			ClickAudioComponent->bIsUISound = true;
			ClickAudioComponent->SetTickableWhenPaused(true);
			ClickAudioComponent->Play();

			FTSTicker::GetCoreTicker().AddTicker(
				FTickerDelegate::CreateLambda(
					[ClickAudioComponent](float DeltaTime)
					{
						if (IsValid(ClickAudioComponent) && ClickAudioComponent->IsPlaying())
						{
							ClickAudioComponent->FadeOut(0.7f, 0.0f);
						}
						return false;
					})
				, 0.0f);
		}
	}

	FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateLambda(
			[this](float DeltaTime)
			{
				ExecuteStartTransition();
				return false;
			})
		, 0.7f);
}

void USanzoMainWidget::HandleExitButtonClicked()
{
	if (QuitSound)
	{
		UAudioComponent* ClickAudioComponent = UGameplayStatics::SpawnSound2D(this, QuitSound);
		
		if (ClickAudioComponent)
		{
			ClickAudioComponent->bIsUISound = true;
			ClickAudioComponent->SetTickableWhenPaused(true);
			ClickAudioComponent->Play();

			FTSTicker::GetCoreTicker().AddTicker(
				FTickerDelegate::CreateLambda(
					[ClickAudioComponent](float DeltaTime)
					{
						if (IsValid(ClickAudioComponent) && ClickAudioComponent->IsPlaying())
						{
							ClickAudioComponent->FadeOut(0.7f, 0.0f);
						}
						return false;
					})
				, 0.0f);
		}
	}

	FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateLambda(
			[this](float DeltaTime)
			{
				ExecuteExitTransition();
				return false;
			})
		, 0.7f);

}

void USanzoMainWidget::PlayGameOverSound()
{
	if (GameOverSound)
	{
		PlayingAudio = UGameplayStatics::SpawnSound2D(this, GameOverSound);
		
		if (PlayingAudio)
		{
			PlayingAudio->bIsUISound = true;
			
			PlayingAudio->SetTickableWhenPaused(true);
			
			PlayingAudio->Play();		
		}
	}
}

#pragma region Transition

void USanzoMainWidget::ExecuteStartTransition()
{
	OnButtonClicked.Broadcast(StartGameTag);
}

void USanzoMainWidget::ExecuteExitTransition()
{
	if (CurrentState == FGameplayTag::RequestGameplayTag(FName("Game.State.MainMenu")))
	{
		OnButtonClicked.Broadcast(QuitGameTag);
	}
	if (CurrentState == FGameplayTag::RequestGameplayTag(FName("Room.State.Cleared")))
	{
		OnButtonClicked.Broadcast(NextStageTag);
	}
	if (CurrentState == FGameplayTag::RequestGameplayTag(FName("Game.State.GameOver")))
	{
		OnButtonClicked.Broadcast(ReturnMainMenuTag);
	}
}

#pragma endregion 이준로
