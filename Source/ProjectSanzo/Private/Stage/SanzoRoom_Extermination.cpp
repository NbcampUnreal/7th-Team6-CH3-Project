#include "Stage/SanzoRoom_Extermination.h"
#include "Stage/SanzoRoomBase.h"
#include "Common/SanzoLog.h"
#include "Core/SanzoGameState.h"

#pragma region Battle Flow for Extermination
void ASanzoRoom_Extermination::BeginRoomSequence()
{
  Super::BeginRoomSequence();
  UE_LOG(LogCYS, Warning, TEXT("섬멸: 시퀀스 시작"));

  // 몬스터 스폰
  EnemySpawned();
  UE_LOG(LogCYS, Warning, TEXT("섬멸: 현재 적 수 - %d"), TotalEnemyCount);
  // 클리어 조건 달성 시 end
  UE_LOG(LogCYS, Warning, TEXT("섬멸: 클리어 조건 - 모든 적 처치"));
  
  //EndRoomSequence();
}

// 조건 클리어 시 호출 함수
void ASanzoRoom_Extermination::EndRoomSequence()
{
  Super::EndRoomSequence();
  OnRoomCleared.Broadcast();
  // 문 열림 호출
  if (!StageGate)
  {
    UE_LOG(LogCYS, Error, TEXT("섬멸: StageGate is null"));
    return;
  }
  StageGate->OpenGate();
}
#pragma endregion 최윤서

#pragma region Battle Event
void ASanzoRoom_Extermination::OnEnemyKilled()
{
  CurrentEnemyCount++;
  if (GameState)
  {
    GameState->UpdateStageInfo(CurrentEnemyCount, TotalEnemyCount);
  }
  UE_LOG(LogCYS, Warning, TEXT("섬멸: 적 처치 %d / %d"), CurrentEnemyCount, TotalEnemyCount);
  if (CurrentEnemyCount >= TotalEnemyCount)
  {
    UE_LOG(LogCYS, Warning, TEXT("섬멸: 모든 적 처치 완료"));
    EndRoomSequence();
  }
}
#pragma endregion 최윤서