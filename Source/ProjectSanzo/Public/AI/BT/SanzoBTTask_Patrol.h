// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "SanzoBTTask_Patrol.generated.h"

UCLASS()
class PROJECTSANZO_API USanzoBTTask_Patrol : public UBTTask_BlackboardBase
{
  GENERATED_BODY()

public:
  USanzoBTTask_Patrol();
  virtual EBTNodeResult::Type ExecuteTask
  (
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory
  ) override;
private:
  // 정찰 반경
  UPROPERTY(EditAnywhere, Category = "AI|Patrol")
  float SearchRadius = 500.0f;
};
