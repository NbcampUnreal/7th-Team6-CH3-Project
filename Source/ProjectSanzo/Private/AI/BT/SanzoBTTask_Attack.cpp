#include "AI/BT/SanzoBTTask_Attack.h"
#include "AIController.h"
#include "AI/SanzoEnemyBase.h"
#include "BehaviorTree/BlackboardComponent.h"

USanzoBTTask_Attack::USanzoBTTask_Attack()
{
  NodeName = TEXT("Attack");
}

EBTNodeResult::Type USanzoBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
  AAIController* AIController = OwnerComp.GetAIOwner();
  if (!AIController) return EBTNodeResult::Failed;

  ASanzoEnemyBase* Enemy = Cast<ASanzoEnemyBase>(AIController->GetPawn());
  if (!Enemy) return EBTNodeResult::Failed;

  UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
  if (!BlackboardComp) return EBTNodeResult::Failed;

  AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TEXT("TargetActor")));

  // 시야에 타겟이 똑바로 보일 때만 공격
  if (TargetActor && AIController->LineOfSightTo(TargetActor))
  {
    Enemy->Attack();
    return EBTNodeResult::Succeeded;
  }

  // 벽에 가려져 있거나 타겟이 없다면 공격하지 않고 실패 반환
  return EBTNodeResult::Failed;
}
