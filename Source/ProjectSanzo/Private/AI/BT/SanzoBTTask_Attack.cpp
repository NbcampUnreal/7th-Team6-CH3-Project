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
    // 초근접 시 콜리전 겹침으로 시야 판정이 실패하는 버그 방지
    float DistanceToTarget = FVector::Distance(Enemy->GetActorLocation(), TargetActor->GetActorLocation());
    bool bCanSee = AIController->LineOfSightTo(TargetActor);

    // 플레이어가 시야에 보이거나, 코앞(250유닛 이하)까지 붙었다면 무조건 공격
    if (bCanSee || DistanceToTarget <= 250.0f)
    {
      if (DistanceToTarget > 150.0f)
      {
        FHitResult MuzzleHit;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(Enemy);

        // 대략적인 총구 높이 계산 (가슴 높이 + 캐릭터 앞쪽)
        FVector MuzzleApprox = Enemy->GetActorLocation() + FVector(0.f, 0.f, 60.f) + (Enemy->GetActorForwardVector() * 50.f);
        FVector TargetLoc = TargetActor->GetActorLocation() + FVector(0.f, 0.f, 30.f);

        bool bMuzzleBlocked = Enemy->GetWorld()->LineTraceSingleByChannel(MuzzleHit, MuzzleApprox, TargetLoc, ECC_Visibility, Params);

        if (bMuzzleBlocked)
        {
          AActor* HitActor = MuzzleHit.GetActor();
          // 총구 앞이 플레이어가 아닌 언덕/벽으로 막혀있다면
          if (HitActor && !HitActor->ActorHasTag("Player") && !HitActor->IsA<ASanzoEnemyBase>())
          {
            // 실패를 반환
            return EBTNodeResult::Failed;
          }
        }
      }
      Enemy->Attack();
      return EBTNodeResult::Succeeded;
    }
  }

  // 벽에 가려져 있거나 타겟이 없다면 공격하지 않고 실패 반환
  return EBTNodeResult::Failed;
}
