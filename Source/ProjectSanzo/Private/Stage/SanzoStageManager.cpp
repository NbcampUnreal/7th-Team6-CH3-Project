#include "Stage/SanzoStageManager.h"

#include "Character/SanzoPlayerController.h"
#include "Core/SanzoGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Common/SanzoLog.h"
#include "Core/SanzoGameState.h"

ASanzoStageManager::ASanzoStageManager()
{
  PrimaryActorTick.bCanEverTick = false;

}

void ASanzoStageManager::BeginPlay()
{
  Super::BeginPlay();
  if(AGameModeBase* GameMode=GetWorld()->GetAuthGameMode())
  {
    SanzoGameMode=Cast<ASanzoGameMode>(GameMode);
  }
  if (StageGate)
  {
    StageGate->OnGateEntered.AddUObject(
      this,
      &ASanzoStageManager::MoveToNextRoom
    );
  }
  else
  {
    UE_LOG(LogCYS, Error, TEXT("SM: Stage gate 연결 안됨."));
  }
  PlayerController = UGameplayStatics::GetPlayerController(this, 0);
}

void ASanzoStageManager::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

}

#pragma region Stage Management

void ASanzoStageManager::StartStage()
{
  UE_LOG(LogCYS, Warning, TEXT("SM: 스테이지 시작"));
  // 현재 스테이지 타입 가져오기
  if (SanzoGameMode)
  {
    StageType = SanzoGameMode->CurrentStageType;
  }

  TSubclassOf<ASanzoRoomBase> RoomClassToSpawn = nullptr;
  switch (StageType)
  {
  case ESanzoStageType::None:
      UE_LOG(LogCYS, Warning, TEXT("SM: 전투 타입 - 없음"));
      return;
    case ESanzoStageType::Extermination:
      UE_LOG(LogCYS, Warning, TEXT("SM: 전투 타입 - 섬멸전"));
      RoomClassToSpawn = ExterminationRoomClass;
      break;
    case ESanzoStageType::Survival:
      UE_LOG(LogCYS, Warning, TEXT("SM: 전투 타입 - 방호전"));
      RoomClassToSpawn = SurvivalRoomClass;
      break;
    case ESanzoStageType::Boss:
      UE_LOG(LogCYS, Warning, TEXT("SM: 전투 타입 - 보스"));
      RoomClassToSpawn = BossRoomClass;
      StartBossIntro();
      break;
  }
  if (!RoomClassToSpawn) return;
  // 방 스폰
  CurrentRoom = GetWorld()->SpawnActor<ASanzoRoomBase>(
    RoomClassToSpawn, 
    FVector::ZeroVector, 
    FRotator::ZeroRotator
  );

  if (CurrentRoom)
  {
    // 방 클리어 이벤트 바인딩
    CurrentRoom->OnRoomCleared.AddUObject(
      this,
      &ASanzoStageManager::OnRoomCleared
    );
    SetState(StageActivedTag);
  }
}

void ASanzoStageManager::OnRoomCleared()
{
  UE_LOG(LogCYS, Warning, TEXT("SM: 스테이지 클리어"));
  SetState(StageClearedTag);
}

void ASanzoStageManager::MoveToNextRoom()
{
  if (SanzoGameMode)
  {
    SanzoGameMode->OnStageCleared();
  }
}
#pragma endregion 최윤서

#pragma region Stage State Management

void ASanzoStageManager::SetState(FGameplayTag NewState)
{
  CurrentState = NewState;

  if (NewState == StageActivedTag)
  {
    // 스테이지 활성
    ASanzoGameState* GameState = Cast<ASanzoGameState>(UGameplayStatics::GetActorOfClass(GetWorld(), ASanzoGameState::StaticClass()));
    if (GameState)
    {
      GameState->UpdateStageInit(StageType, CurrentState);
    }
    CurrentRoom->BeginRoomSequence();
  }
  else if (NewState == StageClearedTag)
  {
    // 안개 지우기
    if(Fog)
    {
      Fog->SetActorHiddenInGame(true);
    }
	  // 스테이지 클리어 - 작업자: 이준로
  	if (PlayerController)
  	{
  		if (ASanzoPlayerController* SanzoPlayerController = Cast<ASanzoPlayerController>(PlayerController))
  		{
  			SanzoPlayerController->ShowAnnouncerUI(StageClearedTag);
  		}
  	}
  	StageCleared.Broadcast();
  }
  else if(NewState== MoveNextTag)
  {
    // 다음 방으로 이동
    MoveToNextRoom();
  }
}
void ASanzoStageManager::StartBossIntro()
{
  if (!PlayerController) return;

  // 입력 잠금
  PlayerController->SetIgnoreMoveInput(true);
  PlayerController->SetIgnoreLookInput(true);

  // 보스 카메라 전환
  PlayerController->SetViewTargetWithBlend(BossCamera, 1.0f);

  // 2초 후 플레이어 보여주기
  FTimerHandle TimerHandle;
  GetWorldTimerManager().SetTimer(
    TimerHandle,
    this,
    &ASanzoStageManager::ShowPlayer,
    2.0f,
    false
  );
}
void ASanzoStageManager::ShowPlayer()
{
  if (!PlayerController) return;

  // 플레이어 카메라 전환
  PlayerController->SetViewTargetWithBlend(PlayerCamera, 1.0f);

  // 2초 후 종료
  FTimerHandle TimerHandle;
  GetWorldTimerManager().SetTimer(
    TimerHandle,
    this,
    &ASanzoStageManager::EndIntro,
    2.0f,
    false
  );
}
void ASanzoStageManager::EndIntro()
{
  if (!PlayerController) return;

  APawn* PlayerPawn = PlayerController->GetPawn();

  // 원래 카메라 복귀
  PlayerController->SetViewTargetWithBlend(PlayerPawn, 0.5f);

  // 입력 해제
  PlayerController->SetIgnoreMoveInput(false);
  PlayerController->SetIgnoreLookInput(false);
}
#pragma endregion 최윤서