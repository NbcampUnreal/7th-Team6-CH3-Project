#include "Core/SanzoGameState.h"
#include "Core/SanzoGameInstance.h"
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

	if (UGameInstance* GI = GetGameInstance())
	{
		USanzoGameInstance* SGI = Cast<USanzoGameInstance>(GI);
		if (SGI)
		{
			//CurrentStageIndex = SGI->CurrentLevelIndex;
		}
	}
	
	OpenHUD();
}

#pragma region UpdateStage
void ASanzoGameState::UpdateStageInfo(float Current, float Total)
{
	//UE_LOG(LogCYS, Warning, TEXT("GS: 전투 정보 업데이트"));
	CurrentCount = Current;
	TotalCount = Total;
	UpdateStageProgressBar();
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
#pragma endregion 이준로
