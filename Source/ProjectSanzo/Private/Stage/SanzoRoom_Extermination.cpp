#include "Stage/SanzoRoom_Extermination.h"
#include "Stage/SanzoRoomBase.h"

void ASanzoRoom_Extermination::BeginRoomSequence()
{
  Super::BeginRoomSequence();
  UE_LOG(LogTemp, Warning, TEXT("섬멸전: 시퀀스 시작"));
  // 클리어 조건 달성
  UE_LOG(LogTemp, Warning, TEXT("섬멸전: 클리어 조건 달성"));
  EndRoomSequence();
}

void ASanzoRoom_Extermination::EndRoomSequence()
{
  Super::EndRoomSequence();
}
