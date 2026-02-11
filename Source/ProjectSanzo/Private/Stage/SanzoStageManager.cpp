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
}

void ASanzoStageManager::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

}

void ASanzoStageManager::StartStage()
{
  UE_LOG(LogTemp, Warning, TEXT("SM: 스테이지 시작"));
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

  CurrentRoom = GetWorld()->SpawnActor<ASanzoRoomBase>(RoomClassToSpawn, FVector::ZeroVector, FRotator::ZeroRotator);
  if (CurrentRoom)
  {
    // 방 클리어 이벤트 바인딩
    CurrentRoom->OnRoomCleared.AddUObject(
      this,
      &ASanzoStageManager::OnRoomCleared
    );
    CurrentRoom->BeginRoomSequence();
  }
}

void ASanzoStageManager::OnRoomCleared()
{
  UE_LOG(LogTemp, Warning, TEXT("SM: 스테이지 클리어"));
  if (SGM) 
  {
    SGM->OnStageCleared();
  }
}