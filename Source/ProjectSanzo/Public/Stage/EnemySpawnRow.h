#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "EnemySpawnRow.generated.h"

USTRUCT(BlueprintType)
struct FEnemySpawnRow : public FTableRowBase
{
  GENERATED_BODY()

public:
  // 적 이름
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FName EnemyName;
  // 어떤 적 클래스를 스폰할지
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TSubclassOf<AActor> EnemyClass;
  // 적의 스폰 확률
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  float SpawnChance;
};