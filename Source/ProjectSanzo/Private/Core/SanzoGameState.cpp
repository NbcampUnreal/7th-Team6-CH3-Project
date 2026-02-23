#include "Core/SanzoGameState.h"
#include "Core/SanzoGameInstance.h"
#include "Core/SanzoStageTypes.h"
#include "Character/SanzoPlayerController.h"
#include "Common/SanzoLog.h"

ASanzoGameState::ASanzoGameState()
{
	CurrentStageIndex = 0;
	CurrentCount = 0;
	TotalCount = 0;
}

void ASanzoGameState::BeginPlay()
{
	Super::BeginPlay();

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USanzoGameInstance* SanzoGameInstance = Cast<USanzoGameInstance>(GameInstance);
		if (SanzoGameInstance)
		{
			CurrentStageIndex = SanzoGameInstance->CurrentStageIndex;
		}
	}

	OpenHUD();
	
	GetWorldTimerManager().SetTimer(
		OpenAnnouncerUITimer,
		this,
		&ASanzoGameState::OpenAnnouncerUI,
		0.01f,
		false
	);
	
}

#pragma region UpdateStage
void ASanzoGameState::UpdateStageInfo(float Current, float Total)
{
	//UE_LOG(LogCYS, Warning, TEXT("GS: 전투 정보 업데이트"));
	CurrentCount = Current;
	TotalCount = Total;
	UpdateStageProgressBar();
}

void ASanzoGameState::UpdateStageResult(int32 KillCount, float SurvivalTime)
{
  UE_LOG(LogCYS, Warning, TEXT("GS: 전투 결과 정보 업데이트 Kill: %d, Time: %.1f"),KillCount, SurvivalTime);
  TotalKillCount = KillCount;
  TotalSurvivalTime = SurvivalTime;
}
void ASanzoGameState::UpdateStageInit(ESanzoStageType StageType, FGameplayTag StageState)
{
	CurrentStageType = StageType;
	CurrentStageState = StageState;
}
#pragma endregion 최윤서

#pragma region UI

void ASanzoGameState::UpdateStageProgressBar()
{
	float Percent = (TotalCount > 0) ? (CurrentCount / TotalCount) : 0.0f;
	if (OnStageProgressChanged.IsBound())
	{
		OnStageProgressChanged.Broadcast(Percent);
	}
}

void ASanzoGameState::OpenHUD()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASanzoPlayerController* SanzoPlayerController = Cast<ASanzoPlayerController>(PlayerController))
		{
			UpdateStageProgressBar();
			SanzoPlayerController->ShowGameHUD();
		}
	}
}

void ASanzoGameState::OpenStageClearUI(FGameplayTag State)
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASanzoPlayerController* SanzoPlayerController = Cast<ASanzoPlayerController>(PlayerController))
		{
			SanzoPlayerController->ShowMainUI(State,TotalSurvivalTime,TotalKillCount );
		}
	}
}

void ASanzoGameState::ResumeGame()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASanzoPlayerController* SanzoPlayerController = Cast<ASanzoPlayerController>(PlayerController))
		{
			UE_LOG(LogLJR,Warning,TEXT("스테이지 시작"));
			SanzoPlayerController->ResumeGame();
		}
	}
}

void ASanzoGameState::OpenAnnouncerUI()
{
	GetWorldTimerManager().ClearTimer(OpenAnnouncerUITimer);
	
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASanzoPlayerController* SanzoPlayerController = Cast<ASanzoPlayerController>(PlayerController))
		{
			//진짜 받아온 정보로 수정 필요
			UE_LOG(LogLJR,Warning,TEXT("스테이지 알림 UI 열기"));
			ESanzoStageType StageType = ESanzoStageType::Extermination;
			FGameplayTag State = FGameplayTag::RequestGameplayTag(FName("Room.State.Actived"));
			SanzoPlayerController->ShowAnnouncerUI(State, StageType);
		}
	}
}
#pragma endregion 이준로
