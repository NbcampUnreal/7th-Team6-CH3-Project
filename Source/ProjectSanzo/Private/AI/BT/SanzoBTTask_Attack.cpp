#include "AI/BT/SanzoBTTask_Attack.h"
#include "AIController.h"
#include "AI/SanzoEnemyBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Animation/AnimInstance.h"

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

  // 공격 몽타주를 무시하지 않도록 이동 정지
  AIController->StopMovement();

  // 몽타주가 이미 재생 중이라면 중복 실행 방지
  if (UAnimInstance* AnimInstance = Enemy->GetMesh()->GetAnimInstance())
  {
    if (AnimInstance->IsAnyMontagePlaying())
    {
      return EBTNodeResult::Succeeded;
    }
  }

  UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
  if (!BlackboardComp) return EBTNodeResult::Failed;

  AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TEXT("TargetActor")));

  if (TargetActor)
  {
    if (Enemy->CanAttack(TargetActor))
    {
      Enemy->Attack();
      return EBTNodeResult::Succeeded;
    }
  }
  return EBTNodeResult::Failed;
}
