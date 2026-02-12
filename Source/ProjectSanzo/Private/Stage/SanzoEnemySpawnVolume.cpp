#include "Stage/SanzoEnemySpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

#pragma region 적 스폰

ASanzoEnemySpawnVolume::ASanzoEnemySpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;
  // 박스 컴포넌트를 생성하고, 이 액터의 루트로 설정
  Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
  SetRootComponent(Scene);

  SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
  SpawningBox->SetupAttachment(Scene);
}

// 랜덤 적 스폰 함수
void ASanzoEnemySpawnVolume::SpawnRandomEnemy()
{
  if (FEnemySpawnRow* SelectedRow = GetRandomEnemy())
  {
    if (UClass* ActualClass = SelectedRow->EnemyClass.Get())
    {
      SpawnEnemy(ActualClass);
    }
  }
}

// 데이터 테이블에서 확률 기반으로 적을 선택
FEnemySpawnRow* ASanzoEnemySpawnVolume::GetRandomEnemy() const
{
  if (!EnemyDataTable) return nullptr;

  TArray<FEnemySpawnRow*> AllRows;
  static const FString ContextString(TEXT("Enemy Spawn Context"));
  EnemyDataTable->GetAllRows<FEnemySpawnRow>(ContextString, AllRows);

  if (AllRows.IsEmpty()) return nullptr;

  float TotalChance = 0.0f;
  for (const FEnemySpawnRow* Row : AllRows)
  {
    if (Row)
    {
      TotalChance += Row->SpawnChance;
    }
  }
  const float RandomValue = FMath::FRandRange(0.0f, TotalChance);
  float AccumulatedChance = 0.0f;

  for(FEnemySpawnRow* Row : AllRows)
  {
    if (Row)
    {
      AccumulatedChance += Row->SpawnChance;
      if (RandomValue <= AccumulatedChance)
      {
        return Row;
      }
    }
  }
  return nullptr;
}

// 특정 적 스폰 함수
void ASanzoEnemySpawnVolume::SpawnEnemy(TSubclassOf<ACharacter> EnemyClass)
{
  if (!EnemyClass) return;

  GetWorld()->SpawnActor<ACharacter>(
    EnemyClass,
    SpawningBox->GetComponentLocation(),
    FRotator::ZeroRotator
  );
}


#pragma endregion 최윤서