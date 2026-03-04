#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Items/ItemSpawnRow.h"
#include "SanzoItemSpawn.generated.h"

#pragma region Item Spawn
UCLASS()
class PROJECTSANZO_API ASanzoItemSpawn : public AActor
{
	GENERATED_BODY()
	
public:	
	ASanzoItemSpawn();
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void SpawnRandomItem(FVector Position);

	//레벨에서 지정 필수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	UDataTable* ItemDataTable;

	FItemSpawnRow* GetRandomItem() const;

	// 특정 적 클래스를 스폰하는 함수
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void SpawnItem(TSubclassOf<AActor> ItemClass, FVector Position);
};
#pragma endregion 최윤서