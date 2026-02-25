#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraComponent.h"
#include "SanzoItemBase.generated.h"

class USphereComponent;

UCLASS()
class PROJECTSANZO_API ASanzoItemBase : public AActor
{
  GENERATED_BODY()

public:
  ASanzoItemBase();
#pragma region ItemBase
protected:
  void BeginPlay() override;
  void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName ItemType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	USceneComponent* Scene;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	USphereComponent* Collision;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	UStaticMeshComponent* StaticMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	UNiagaraComponent* NiagaraComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	UNiagaraSystem* SpawnEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	USoundBase* PickupSound;
	UFUNCTION()
	virtual void OnItemOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	virtual void ActivateItem(AActor* Activator);
	virtual void DestroyItem();
#pragma endregion 최윤서
};
