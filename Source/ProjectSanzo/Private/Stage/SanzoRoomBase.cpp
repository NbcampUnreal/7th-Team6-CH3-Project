#include "Stage/SanzoRoomBase.h"
#include "Stage/SanzoEnemySpawnVolume.h"
#include "Kismet/GameplayStatics.h"
#include "Common/SanzoLog.h"

#pragma region 전투 Base

ASanzoRoomBase::ASanzoRoomBase()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ASanzoRoomBase::BeginPlay()
{
	Super::BeginPlay();

	// 문 찾기
	TArray<AActor*> FoundGates;
	UGameplayStatics::GetAllActorsOfClass(
		GetWorld(),
		ASanzoStageGate::StaticClass(),
		FoundGates
	);
	if (FoundGates.Num() > 0)
	{
		StageGate = Cast<ASanzoStageGate>(FoundGates[0]);
		UE_LOG(LogCYS, Warning, TEXT("RB: Gate Found"));
	}
	else
	{
		UE_LOG(LogCYS, Error, TEXT("RB: Gate Not Found"));
	}

  // 스폰 볼륨 찾기
	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(
		GetWorld(), 
		ASanzoEnemySpawnVolume::StaticClass(), 
		FoundVolumes
	);
	for (AActor* Actor : FoundVolumes)
	{
		if (ASanzoEnemySpawnVolume* Volume = Cast<ASanzoEnemySpawnVolume>(Actor))
		{
			SpawnVolumes.Add(Volume);
      UE_LOG(LogCYS, Warning, TEXT("RB: Spawn Volume Found"));
		}
	}

}

void ASanzoRoomBase::BeginRoomSequence()
{
	// 문 잠금, 스폰 시작
	UE_LOG(LogCYS, Warning, TEXT("RB: 시퀀스 시작"));


}

void ASanzoRoomBase::EndRoomSequence()
{
    // 문 열기, 다음 방 개방 지시
	UE_LOG(LogCYS, Warning, TEXT("RB: 시퀀스 종료"));
}

void ASanzoRoomBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASanzoRoomBase::OnEnemyKilled()
{
	// 기본은 아무것도 안 함
}
//  적 스폰 호출
void ASanzoRoomBase::EnemySpawned()
{
	for(ASanzoEnemySpawnVolume* SpawnVolume:SpawnVolumes)
	{
		if (SpawnVolume)
		{
			SpawnVolume->SpawnRandomEnemy();
		}
  }
}
#pragma endregion 최윤서
