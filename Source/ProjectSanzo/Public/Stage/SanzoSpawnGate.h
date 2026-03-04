// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "SanzoSpawnGate.generated.h"

UCLASS()
class PROJECTSANZO_API ASanzoSpawnGate : public AActor
{
	GENERATED_BODY()
#pragma region Stage Gate
public:
  ASanzoSpawnGate();

  void OpenGate();
  void CloseGate();

protected:
  virtual void BeginPlay() override;

  UPROPERTY(VisibleAnywhere)
  USceneComponent* Root;

  UPROPERTY(VisibleAnywhere)
  UStaticMeshComponent* GateMeshL;
  UPROPERTY(VisibleAnywhere)
  UStaticMeshComponent* GateMeshR;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
  USoundBase* OpenSound;
  UPROPERTY(EditAnywhere, Category = "Effects")
  USoundAttenuation* OpenSoundAttenuation;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
  USoundBase* CloseSound;

  UPROPERTY(VisibleAnywhere)
  UBoxComponent* TriggerBox;

  FTimerHandle DoorTimer;

  float RotateSpeed = 180.f;   // deg/sec
  float RotatedAmount = 0.f;   // 누적 회전량
  float TargetAngle = 90.f;
  FRotator LeftRot;
  FRotator RightRot;

private:
  UFUNCTION()
  void OnOverlapEnd(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex
  );
  UFUNCTION()
  void OnOverlapBegin(
    UPrimitiveComponent* OverlappedComponent, 
    AActor* OtherActor, 
    UPrimitiveComponent* OtherComp, 
    int32 OtherBodyIndex, 
    bool bFromSweep, 
    const FHitResult& SweepResult
  );
  void RotateDoor();

  bool bIsOpened = false;
#pragma endregion 최윤서
};
