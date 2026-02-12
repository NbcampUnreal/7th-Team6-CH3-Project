#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Stage/EnemySpawnRow.h"
#include "SanzoEnemySpawnVolume.generated.h"

#pragma region 적 스폰
class UBoxComponent;

UCLASS()
class PROJECTSANZO_API ASanzoEnemySpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASanzoEnemySpawnVolume();


	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void SpawnRandomEnemy();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	USceneComponent* Scene;
	// 스폰 영역을 담당할 박스 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	UBoxComponent* SpawningBox;

	//레벨에서 지정 필수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	UDataTable* EnemyDataTable;

	FEnemySpawnRow* GetRandomEnemy() const;

	// 특정 적 클래스를 스폰하는 함수
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void SpawnEnemy(TSubclassOf<ACharacter> EnemyClass);

};
#pragma endregion 최윤서