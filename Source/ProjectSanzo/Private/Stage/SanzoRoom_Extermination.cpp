#include "Stage/SanzoRoom_Extermination.h"
#include "Stage/SanzoRoomBase.h"

void ASanzoRoom_Extermination::BeginRoomSequence()
{
  Super::BeginRoomSequence();
  UE_LOG(LogTemp, Warning, TEXT("섬멸전: 시퀀스 시작"));

  // 몬스터 스폰
  // 
  // 
  // 클리어 조건 달성 시 end
  UE_LOG(LogTemp, Warning, TEXT("섬멸전: 클리어 조건 체크"));
  // 더미 조건: 10초 후 클리어
  GetWorld()->GetTimerManager().SetTimer(
    RoomSequenceTimerHandle,
    this,
    &ASanzoRoom_Extermination::EndRoomSequence,
    10.0f,
    false
  );
  //EndRoomSequence();
}

// 조건 클리어 시 함수 호출
void ASanzoRoom_Extermination::EndRoomSequence()
{
  Super::EndRoomSequence();
  OnRoomCleared.Broadcast();
  // 문 열림 호출
  if (!StageGate)
  {
    UE_LOG(LogTemp, Error, TEXT("섬멸전: StageGate is null"));
    return;
  }
  StageGate->OpenGate();
}

void ASanzoRoom_Extermination::OnEnemyKilled()
{
  CurrentEnemyCount++;
  UE_LOG(LogTemp, Warning, TEXT("섬멸전: 적 처치 %d / %d"), CurrentEnemyCount, TotalEnemyCount);
  if (CurrentEnemyCount >= TotalEnemyCount)
  {
    UE_LOG(LogTemp, Warning, TEXT("섬멸전: 모든 적 처치 완료"));
    EndRoomSequence();
  }
}
