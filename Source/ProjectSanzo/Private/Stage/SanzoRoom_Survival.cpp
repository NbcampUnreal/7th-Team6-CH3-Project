#include "Stage/SanzoRoom_Survival.h"
#include "Common/SanzoLog.h"

#pragma region 방호전 배틀 흐름
void ASanzoRoom_Survival::BeginRoomSequence()
{
  Super::BeginRoomSequence();
  UE_LOG(LogCYS, Warning, TEXT("방호: 시퀀스 시작"));
  // 클리어 조건 달성 시 end
  UE_LOG(LogCYS, Warning, TEXT("방호: 클리어 조건 체크"));
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
#pragma endregion 최윤서

#pragma region 배틀 내 이벤트
#pragma endregion 최윤서