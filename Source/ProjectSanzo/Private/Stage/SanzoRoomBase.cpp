#include "Stage/SanzoRoomBase.h"
#include "Stage/SanzoEnemySpawnVolume.h"
#include "Items/SanzoItemSpawn.h"
#include "Kismet/GameplayStatics.h"
#include "Common/SanzoLog.h"
#include "Core/SanzoGameState.h"

#pragma region Room Base

ASanzoRoomBase::ASanzoRoomBase()
{
	PrimaryActorTick.bCanEverTick = false;

	TotalEnemyCount = 0;
  CurrentEnemyCount = 0;
  CurrentTime = 0.0f;
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
		////UE_LOG(LogCYS, Warning, TEXT("RB: Gate Found"));
	}
	else
	{
		////UE_LOG(LogCYS, Error, TEXT("RB: Gate Not Found"));
	}

  // 적 스폰 볼륨 찾기
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
      ////UE_LOG(LogCYS, Warning, TEXT("RB: Spawn Volume Found"));
		}
	}

	// Game State 찾기
	ASanzoGameState* Found = Cast<ASanzoGameState>(UGameplayStatics::GetActorOfClass(GetWorld(), ASanzoGameState::StaticClass()));
	if (Found)
	{
		GameState = Found;
	}
	
	// 아이템 스폰 찾기
	AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), ASanzoItemSpawn::StaticClass());
	ItemSpawn = Cast<ASanzoItemSpawn>(FoundActor);
	if (ItemSpawn)
	{
    ////UE_LOG(LogCYS, Warning, TEXT("RB: Item Spawn Found"));
	}
}

void ASanzoRoomBase::BeginRoomSequence()
{
	// 문 잠금, 스폰 시작
	////UE_LOG(LogCYS, Warning, TEXT("RB: 시퀀스 시작"));


}

void ASanzoRoomBase::EndRoomSequence()
{
	// 클리어 알림
	OnRoomCleared.Broadcast();
  // 타이머 정지
	GetWorld()->GetTimerManager().ClearTimer(RoomSequenceTimerHandle);

    // 문 열기
	////UE_LOG(LogCYS, Warning, TEXT("RB: 시퀀스 종료"));
	if(GameState)
	{
		GameState->UpdateStageResult(CurrentEnemyCount, CurrentTime);
  }
}

void ASanzoRoomBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASanzoRoomBase::OnEnemyKilled(FVector Position)
{
	// 처치 수 카운트
	CurrentEnemyCount++;
	// 아이템 드롭
	if (ItemSpawn)
	{
    ItemSpawn->SpawnRandomItem(Position);
	}
}

void ASanzoRoomBase::UpdateTime()
{
  // 시간 업데이트
	CurrentTime += 0.1;
}

//  적 스폰 호출
void ASanzoRoomBase::EnemySpawned()
{
	for(ASanzoEnemySpawnVolume* SpawnVolume:SpawnVolumes)
	{
		if (SpawnVolume)
		{
			SpawnVolume->SpawnRandomEnemy();
			TotalEnemyCount += 1;
		}
  }
}
#pragma endregion 최윤서
