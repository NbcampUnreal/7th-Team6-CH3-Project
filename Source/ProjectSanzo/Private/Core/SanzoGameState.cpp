#include "Core/SanzoGameState.h"
#include "Core/SanzoGameInstance.h"
#include "Character/SanzoPlayerController.h"

ASanzoGameState::ASanzoGameState()
{
	CurrentStageIndex = 0;
	CurrentCount = 0;
	TotalCount = 100.f;
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

	//테스트 코드
	GetWorldTimerManager().SetTimer(
		UpdateStageProgressBarTimer,
		this,
		&ASanzoGameState::AddCurrentCount,
		0.1f,
		true
	);
}

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
			SanzoPlayerController->ShowGameHUD();
		}
	}
}

//테스트 코드
void ASanzoGameState::AddCurrentCount()
{
	CurrentCount++;
	UpdateStageProgressBar();

	if (CurrentCount >= TotalCount)
	{
		CurrentCount = 0;
	}
}
#pragma endregion 이준로
