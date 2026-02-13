#include "Stage/SanzoRoom_Survival.h"
#include "Common/SanzoLog.h"
#include "Core/SanzoGameState.h"

#pragma region Battle Flow for Survival
ASanzoRoom_Survival::ASanzoRoom_Survival()
{
  // 클리어 조건 시간 (더미)
  TotalTime = 30.0;
}
void ASanzoRoom_Survival::BeginRoomSequence()
{
  Super::BeginRoomSequence();
  UE_LOG(LogCYS, Warning, TEXT("방호: 시퀀스 시작"));
  // 적 스폰
  EnemySpawned();
  // 클리어 조건 달성 시 end
  UE_LOG(LogCYS, Warning, TEXT("방호: 클리어 조건 - 제한 시간 %.1f초 동안 살아남기"), TotalTime);
  GetWorldTimerManager().SetTimer(
    RoomSequenceTimerHandle,
    this,
    &ASanzoRoom_Survival::UpdateTime,
    0.1,
    true
  );
}
void ASanzoRoom_Survival::EndRoomSequence()
{
  Super::EndRoomSequence();
  OnRoomCleared.Broadcast();
  // 문 열림 호출
  if (!StageGate)
  {
    UE_LOG(LogCYS, Error, TEXT("방호: StageGate is null"));
    return;
  }
  StageGate->OpenGate();
}
#pragma endregion 최윤서

#pragma region Battle Event
void ASanzoRoom_Survival::UpdateTime()
{
  CurrentTime += 0.1;
  if (GameState)
  {
    GameState->UpdateStageInfo(CurrentTime, TotalTime);
  }
  if (CurrentTime > TotalTime)
  {
    UE_LOG(LogCYS, Warning, TEXT("방호: 시간 끝"));
    GetWorld()->GetTimerManager().ClearTimer(RoomSequenceTimerHandle);
    EndRoomSequence();
  }
}
#pragma endregion 최윤서