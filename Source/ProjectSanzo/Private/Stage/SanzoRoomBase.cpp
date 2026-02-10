#include "Stage/SanzoRoomBase.h"


ASanzoRoomBase::ASanzoRoomBase()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ASanzoRoomBase::BeginPlay()
{
	Super::BeginPlay();
}

void ASanzoRoomBase::BeginRoomSequence()
{
	// 문 잠금, 스폰 시작
	UE_LOG(LogTemp, Warning, TEXT("RB: 시퀀스 시작"));
	RoomStateTags.AddTag(FGameplayTag::RequestGameplayTag("Room.State.Actived"));

	// 클리어 조건 달성
  UE_LOG(LogTemp, Warning, TEXT("RB: 클리어 조건 달성"));
  EndRoomSequence();
}

void ASanzoRoomBase::EndRoomSequence()
{
    // 문 열기, 다음 방 개방 지시
	UE_LOG(LogTemp, Warning, TEXT("RB: 시퀀스 종료"));
	OnRoomCleared.Broadcast();
}

void ASanzoRoomBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

