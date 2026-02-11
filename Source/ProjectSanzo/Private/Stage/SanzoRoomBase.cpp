#include "Stage/SanzoRoomBase.h"
#include "Kismet/GameplayStatics.h"

ASanzoRoomBase::ASanzoRoomBase()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ASanzoRoomBase::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundGates;
	UGameplayStatics::GetAllActorsOfClass(
		GetWorld(),
		ASanzoStageGate::StaticClass(),
		FoundGates
	);
	if (FoundGates.Num() > 0)
	{
		StageGate = Cast<ASanzoStageGate>(FoundGates[0]);
		UE_LOG(LogTemp, Warning, TEXT("RB: Gate Found"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("RB: Gate Not Found"));
	}
}

void ASanzoRoomBase::BeginRoomSequence()
{
	// 문 잠금, 스폰 시작
	UE_LOG(LogTemp, Warning, TEXT("RB: 시퀀스 시작"));


}

void ASanzoRoomBase::EndRoomSequence()
{
    // 문 열기, 다음 방 개방 지시
	UE_LOG(LogTemp, Warning, TEXT("RB: 시퀀스 종료"));
}

void ASanzoRoomBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

