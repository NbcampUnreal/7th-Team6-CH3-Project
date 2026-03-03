#include "Core/SanzoGameMode.h"
#include "Core/SanzoGameState.h"
#include "Core/SanzoStageTypes.h"
#include "Character/SanzoCharacter.h"
#include "Character/SanzoPlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "Stage/SanzoStageManager.h"
#include "EngineUtils.h"
#include "Common/SanzoLog.h"
#include "Core/UpgradeSystem/SanzoUpgradeSubsystem.h"
#include "Kismet/GameplayStatics.h"

ASanzoGameMode::ASanzoGameMode()
{
    DefaultPawnClass = ASanzoCharacter::StaticClass();
    PlayerControllerClass = ASanzoPlayerController::StaticClass();
    GameStateClass = ASanzoGameState::StaticClass();
}

void ASanzoGameMode::BeginPlay()
{
  Super::BeginPlay();
  SanzoGameInstance = Cast<USanzoGameInstance>(GetGameInstance());
  InitStageType();
  if(CurrentStageType!=ESanzoStageType::None)
  {
    // 스테이지 시작
    StartStage();
  }
}
#pragma region Stage Management
// 맵 이름에 따라 스테이지 타입 설정
void ASanzoGameMode::InitStageType()
{
  if (GetWorld()->GetMapName().Contains("Stage1"))
  {
    UE_LOG(LogCYS, Warning, TEXT("GM: 스테이지 타입 설정 - 섬멸전"));
    CurrentStageType = ESanzoStageType::Extermination;
  }
  else if (GetWorld()->GetMapName().Contains("Stage2"))
  {
    UE_LOG(LogCYS, Warning, TEXT("GM: 스테이지 타입 설정 - 방호전"));
    CurrentStageType = ESanzoStageType::Survival;
  }
  else if (GetWorld()->GetMapName().Contains("Stage3"))
  {
    UE_LOG(LogCYS, Warning, TEXT("GM: 스테이지 타입 설정 - 보스전"));
    CurrentStageType = ESanzoStageType::Boss;
  }
  else
  {
    UE_LOG(LogCYS, Warning, TEXT("GM: 스테이지 타입 없음"));
    CurrentStageType = ESanzoStageType::None;
  }
}
void ASanzoGameMode::StartStage()
{
  // StageManager에게 스테이지 시작 지시
  UE_LOG(LogCYS, Warning, TEXT("GM: 스테이지 시작"));
  ASanzoStageManager* StageManager = nullptr;
  for (TActorIterator<ASanzoStageManager> It(GetWorld()); It; ++It)
  {
    StageManager = *It;
    break;
  }
  if (StageManager)
  {
    StageManager->StartStage();
  }
}
void ASanzoGameMode::OnStageCleared()
{
  UE_LOG(LogCYS, Warning, TEXT("GM: 스테이지 클리어"));
  if(CurrentStageType == ESanzoStageType::Boss)
  {
  	DecideEnding();
    MoveToNextStage();
    return;
  }
	ASanzoGameState* SanzoGameState = GetWorld() ? GetWorld()->GetGameState<ASanzoGameState>() : nullptr;
	if (SanzoGameState)
	{
		UE_LOG(LogLJR, Warning, TEXT("SM: 결과창 띄우기"));
		SanzoGameState->OpenStageClearUI(FGameplayTag::RequestGameplayTag(FName("Room.State.Cleared")));
	}
}

//함수 분리
void ASanzoGameMode::MoveToNextStage()
{
	// 스테이지 이동
	if (SanzoGameInstance)
	{
    SanzoGameInstance->MoveToNextStage();
	}
}

#pragma endregion 최윤서

#pragma region Upgrade Management
// 업그레이드 선택 트리거, 캐릭터에서 호출
void ASanzoGameMode::TriggerUpgradeSelection()
{
  UE_LOG(LogCYS, Warning, TEXT("GM: 캐릭터 성장 선택"));

  OnUpgradeSelected();
}

void ASanzoGameMode::OnUpgradeSelected()
{
  UE_LOG(LogCYS, Warning, TEXT("GM: 캐릭터 성장 선택 표시"));

}
#pragma endregion 최윤서

#pragma region Ending Decision

void ASanzoGameMode::DecideEnding()
{
	if (SanzoGameInstance)
	{
		USanzoUpgradeSubsystem* UpgradeSubsystem = SanzoGameInstance->GetSubsystem<USanzoUpgradeSubsystem>();
		if (UpgradeSubsystem)
		{
			const TMap<FName, int32> SelectedTotalMap = UpgradeSubsystem->GetSelectedTotalMap();
			const int32* BeautyValuePtr = SelectedTotalMap.Find(FName("Chr_BTY_Li"));
			if (BeautyValuePtr && *BeautyValuePtr >= 3)
			{
				UE_LOG(LogLJR, Warning, TEXT("외모 3단계 업그레이드 확인 진엔딩 태그 적용"))
				SanzoGameInstance->MediaPlayTag = FGameplayTag::RequestGameplayTag(FName("UI.State.TrueEnding"));
			}
			else
			{
				UE_LOG(LogLJR, Warning, TEXT("외모 3단계 업그레이드 아님 그냥엔딩 태그 적용"))
				SanzoGameInstance->MediaPlayTag = FGameplayTag::RequestGameplayTag(FName("UI.State.Ending"));
			}
		}
	}
}

#pragma endregion 이준로