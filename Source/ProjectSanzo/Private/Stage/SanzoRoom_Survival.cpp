#include "Stage/SanzoRoom_Survival.h"
#include "Common/SanzoLog.h"
#include "Core/SanzoGameState.h"
#include "Kismet/GameplayStatics.h"

#pragma region Battle Flow for Survival
ASanzoRoom_Survival::ASanzoRoom_Survival()
{
  // 클리어 조건 시간 (더미)
  TotalTime = 120.0;
}
void ASanzoRoom_Survival::BeginPlay()
{
  Super::BeginPlay();
}
void ASanzoRoom_Survival::BeginRoomSequence()
{
  Super::BeginRoomSequence(); 
  // 클리어 조건 달성 시 end
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
  // 문 열림 호출
  if (!StageGate)
  {
    return;
  }
  StageGate->OpenGate();
}
#pragma endregion 최윤서

#pragma region Battle Event
void ASanzoRoom_Survival::UpdateTime()
{
  Super::UpdateTime();
  if (GameState)
  {
    GameState->UpdateStageInfo(CurrentTime, TotalTime);
  }
  ElapsedTime += 0.1;
  if(ElapsedTime>=CycleTime)
  {
    ElapsedTime = 0.f;
    EnemySpawned();
    EnemySpawned();
  }
  if (CurrentTime > TotalTime)
  {
    EndRoomSequence();
  }
}
#pragma endregion 최윤서