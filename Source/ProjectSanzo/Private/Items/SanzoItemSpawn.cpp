#include "Items/SanzoItemSpawn.h"
#include "Common/SanzoLog.h"

#pragma region Item Spawn

ASanzoItemSpawn::ASanzoItemSpawn()
{
	PrimaryActorTick.bCanEverTick = false;

}
// 랜덤 아이템 스폰 함수
void ASanzoItemSpawn::SpawnRandomItem(FVector Position)
{
	if(FItemSpawnRow* SelectedRow = GetRandomItem())
	{
		if(UClass* ActualClass = SelectedRow->ItemClass.Get())
		{
			SpawnItem(ActualClass,Position);
		}
  }
}

FItemSpawnRow* ASanzoItemSpawn::GetRandomItem() const
{
	if (!ItemDataTable)
	{
		UE_LOG(LogCYS, Error, TEXT("IS: Data Table 연결 안됨"));
		return nullptr;
	}
	TArray<FItemSpawnRow*> AllRows;
	static const FString ContextString(TEXT("Item Spawn Context"));
	ItemDataTable->GetAllRows<FItemSpawnRow>(ContextString, AllRows);
	if (AllRows.IsEmpty()) return nullptr;
	float TotalChance = 0.0f;
	for (const FItemSpawnRow* Row : AllRows)
	{
		if (Row)
		{
			TotalChance += Row->SpawnChance;
		}
	}
	const float RandomValue = FMath::FRandRange(0.0f, TotalChance);
	float AccumulatedChance = 0.0f;
	for(FItemSpawnRow* Row : AllRows)
	{
		if(Row)
		{
			AccumulatedChance += Row->SpawnChance;
			if(RandomValue <= AccumulatedChance)
			{
				return Row;
			}
		}
  }
	return nullptr;
}

void ASanzoItemSpawn::SpawnItem(TSubclassOf<AActor> ItemClass, FVector Position)
{
	if(!ItemClass)
	{
		UE_LOG(LogCYS, Error, TEXT("IS: 유효하지 않은 아이템 클래스"));
		return;
	}
  GetWorld()->SpawnActor<AActor>(
		ItemClass, 
		Position, 
		FRotator::ZeroRotator);
}
#pragma endregion 최윤서