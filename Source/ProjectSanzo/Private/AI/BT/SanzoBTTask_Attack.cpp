// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/SanzoBTTask_Attack.h"
#include "AIController.h"
#include "AI/SanzoEnemyBase.h"

USanzoBTTask_Attack::USanzoBTTask_Attack()
{
  NodeName = TEXT("Attack");
}

EBTNodeResult::Type USanzoBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
  AAIController* AIController = OwnerComp.GetAIOwner();
  ASanzoEnemyBase* Enemy = Cast<ASanzoEnemyBase>(AIController->GetPawn());

  if (Enemy)
  {
    Enemy->Attack();

    return EBTNodeResult::Succeeded;
  }
  return EBTNodeResult::Failed;
}
