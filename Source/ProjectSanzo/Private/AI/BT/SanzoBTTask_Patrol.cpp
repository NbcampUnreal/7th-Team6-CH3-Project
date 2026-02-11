// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/SanzoBTTask_Patrol.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

USanzoBTTask_Patrol::USanzoBTTask_Patrol()
{
  NodeName = TEXT("Find Patrol Pos");
}

EBTNodeResult::Type USanzoBTTask_Patrol::ExecuteTask
(
  UBehaviorTreeComponent& OwnerComp,
  uint8* NodeMemory
)
{
  AAIController* AIController = OwnerComp.GetAIOwner();
  APawn* ControllingPawn = AIController ? AIController->GetPawn() : nullptr;

  if (ControllingPawn)
  {
    UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
    FNavLocation RandomLocation;

    // 내 위치 기준 반경 내 랜덤 위치 찾기
    if (NavSystem &&
      NavSystem->GetRandomPointInNavigableRadius(ControllingPawn
        ->GetActorLocation(),
        SearchRadius,
        RandomLocation))
    {
      // 찾은 위치를 블랙보드(TargetLocation)에 저장
      OwnerComp.GetBlackboardComponent()
        ->SetValueAsVector(GetSelectedBlackboardKey(),
          RandomLocation.Location);
      return EBTNodeResult::Succeeded;
    }
  }
  return EBTNodeResult::Failed;
}

