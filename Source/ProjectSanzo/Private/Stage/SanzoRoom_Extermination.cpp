#include "Stage/SanzoRoom_Extermination.h"
#include "Stage/SanzoRoomBase.h"

void ASanzoRoom_Extermination::BeginRoomSequence()
{
  Super::BeginRoomSequence();
  UE_LOG(LogTemp, Warning, TEXT("섬멸전: 시퀀스 시작"));
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

void ASanzoRoom_Extermination::EndRoomSequence()
{
  Super::EndRoomSequence();
}
