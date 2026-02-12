#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "SanzoStageGate.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnGateEntered);

UCLASS()
class PROJECTSANZO_API ASanzoStageGate : public AActor
{
  GENERATED_BODY()
#pragma region Stage Gate
public:
  ASanzoStageGate();

  void OpenGate();

  FOnGateEntered OnGateEntered;

protected:
  virtual void BeginPlay() override;

  UPROPERTY(VisibleAnywhere)
  USceneComponent* Root;

  UPROPERTY(VisibleAnywhere)
  UStaticMeshComponent* GateMesh;

  UPROPERTY(VisibleAnywhere)
  UBoxComponent* TriggerBox;

private:
  UFUNCTION()
  void OnOverlapBegin(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult
  );

  bool bIsOpened = false;
#pragma endregion 최윤서
};
