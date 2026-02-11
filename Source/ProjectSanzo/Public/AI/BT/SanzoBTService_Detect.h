// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "SanzoBTService_Detect.generated.h"

UCLASS()
class PROJECTSANZO_API USanzoBTService_Detect : public UBTService
{
  GENERATED_BODY()

public:
  USanzoBTService_Detect();

protected:
  virtual void TickNode(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory,
    float DeltaSeconds) override;

  // 걷기 속도
  UPROPERTY(EditAnywhere, Category = "AI|Movement")
  float PatrolSpeed = 300.0f;

  // 달리기 속도
  UPROPERTY(EditAnywhere, Category = "AI|Movement")
  float ChaseSpeed = 500.0f;
};
