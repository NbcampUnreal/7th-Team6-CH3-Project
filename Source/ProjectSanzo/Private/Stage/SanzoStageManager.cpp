#include "Stage/SanzoStageManager.h"
#include "Core/SanzoGameMode.h"
#include "Kismet/GameplayStatics.h"

ASanzoStageManager::ASanzoStageManager()
{
  PrimaryActorTick.bCanEverTick = false;

}

void ASanzoStageManager::BeginPlay()
{
  Super::BeginPlay();
  if(AGameModeBase* GM=GetWorld()->GetAuthGameMode())
  {
    SGM=Cast<ASanzoGameMode>(GM);
  }
  if (StageGate)
  {
    StageGate->OnGateEntered.AddUObject(
      this,
      &ASanzoStageManager::MoveToNextRoom
    );
  }
}

void ASanzoStageManager::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

}

void ASanzoStageManager::StartStage()
{
  UE_LOG(LogTemp, Warning, TEXT("SM: 스테이지 시작"));
  // 현재 스테이지 타입 가져오기
  if (SGM)
  {
    StageType = SGM->CurrentStageType;
  }

  TSubclassOf<ASanzoRoomBase> RoomClassToSpawn = nullptr;
  switch (StageType)
  {
  case ESanzoStageType::None:
      UE_LOG(LogTemp, Warning, TEXT("SM: 전투 타입 - 없음"));
      return;
    case ESanzoStageType::Extermination:
      UE_LOG(LogTemp, Warning, TEXT("SM: 전투 타입 - 섬멸전"));
      RoomClassToSpawn = ExterminationRoomClass;
      break;
    case ESanzoStageType::Survival:
      UE_LOG(LogTemp, Warning, TEXT("SM: 전투 타입 - 버티기"));
      RoomClassToSpawn = SurvivalRoomClass;
      break;
    case ESanzoStageType::Boss:
      UE_LOG(LogTemp, Warning, TEXT("SM: 전투 타입 - 보스"));
      RoomClassToSpawn = BossRoomClass;
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
  UE_LOG(LogTemp, Warning, TEXT("SM: 스테이지 클리어"));
  SetState(StageClearedTag);
}

void ASanzoStageManager::MoveToNextRoom()
{
  if (SGM)
  {
    SGM->OnStageCleared();
  }
}

void ASanzoStageManager::SetState(FGameplayTag NewState)
{
  CurrentState = NewState;

  if (NewState == StageActivedTag)
  {
    // 스테이지 활성
    CurrentRoom->BeginRoomSequence();
  }
  else if (NewState == StageClearedTag)
  {
    // 스테이지 클리어
  }
  else if(NewState== MoveNextTag)
  {
    // 다음 방으로 이동
    MoveToNextRoom();
  }
}