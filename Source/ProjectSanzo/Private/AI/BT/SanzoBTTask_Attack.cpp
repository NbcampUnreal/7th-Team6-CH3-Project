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
    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Enemy);

    // 가슴 높이에서 플레이어의 가슴 높이로 레이저 발사
    FVector Start = Enemy->GetActorLocation() + FVector(0.f, 0.f, 50.f);
    FVector End = TargetActor->GetActorLocation() + FVector(0.f, 0.f, 50.f);

    bool bHit = Enemy->GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

    // 벽에 안 막혔거나
    // 코앞(250 이하)까지 붙어있다면 무조건 공격 시작
    if (!bHit || (Hit.GetActor() && Hit.GetActor()->ActorHasTag("Player")) || FVector::Distance(Start, End) <= 250.f)
    {
      Enemy->Attack();
      return EBTNodeResult::Succeeded;
    }
  }

  // 벽에 가려져 있거나 타겟이 없다면 공격하지 않고 실패 반환
  return EBTNodeResult::Failed;
}
