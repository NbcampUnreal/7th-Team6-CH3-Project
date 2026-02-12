#include "Stage/SanzoRoom_Survival.h"
#include "Common/SanzoLog.h"

void ASanzoRoom_Survival::BeginRoomSequence()
{
  Super::BeginRoomSequence();
  UE_LOG(LogCYS, Warning, TEXT("버티기: 시퀀스 시작"));
  // 클리어 조건 달성 시 end
  UE_LOG(LogCYS, Warning, TEXT("버티기: 클리어 조건 체크"));
  // 더미 조건: 10초 후 클리어
  GetWorld()->GetTimerManager().SetTimer(
    RoomSequenceTimerHandle,
    this,
    &ASanzoRoom_Survival::EndRoomSequence,
    10.0f,
    false
  );
  //EndRoomSequence();
}

void ASanzoRoom_Survival::EndRoomSequence()
{
  Super::EndRoomSequence();

}
