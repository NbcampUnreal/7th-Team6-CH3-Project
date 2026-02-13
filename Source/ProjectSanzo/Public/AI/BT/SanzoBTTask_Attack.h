// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "SanzoBTTask_Attack.generated.h"

UCLASS()
class PROJECTSANZO_API USanzoBTTask_Attack : public UBTTask_BlackboardBase
{
  GENERATED_BODY()
  USanzoBTTask_Attack();
  virtual EBTNodeResult::Type ExecuteTask
  (
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory
  ) override;
};
