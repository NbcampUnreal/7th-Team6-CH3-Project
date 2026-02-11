#include "Stage/SanzoRoom_Survival.h"

void ASanzoRoom_Survival::BeginRoomSequence()
{
  Super::BeginRoomSequence();
  UE_LOG(LogTemp, Warning, TEXT("버티기: 시퀀스 시작"));
  // 클리어 조건 달성
  UE_LOG(LogTemp, Warning, TEXT("버티기: 클리어 조건 달성"));
  EndRoomSequence();
}

void ASanzoRoom_Survival::EndRoomSequence()
{
  Super::EndRoomSequence();

}
