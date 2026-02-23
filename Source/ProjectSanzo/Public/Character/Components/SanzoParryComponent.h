// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SanzoParryComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTSANZO_API USanzoParryComponent : public UActorComponent
{
  GENERATED_BODY()

public:
  USanzoParryComponent();

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parry")
  UAnimMontage* ParryMontage;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parry")
  UAnimMontage* ParrySuccessMontage;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parry")
  USoundBase* ParrySound;

  FTimerHandle SlowTimerHandle;

  int32 ParryCount;

  void PlayParryMontage();
  void SuccessParry();

  //패리몽타주 끝날때 실행할 델리게이트 선언
  FOnMontageBlendingOutStarted BlendingOutDelegate;

protected:
  virtual void BeginPlay() override;
  
  
public:
  virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


};
