#include "AI/BT/SanzoBTTask_PlayMontage.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"

USanzoBTTask_PlayMontage::USanzoBTTask_PlayMontage()
{
  NodeName = TEXT("Play Anim Montage");
}

EBTNodeResult::Type USanzoBTTask_PlayMontage::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
  if (!MontageToPlay)
  {
    return EBTNodeResult::Failed;
  }

  AAIController* AIController = OwnerComp.GetAIOwner();
  if (!AIController) return EBTNodeResult::Failed;

  ACharacter* AICharacter = Cast<ACharacter>(AIController->GetPawn());
  if (!AICharacter) return EBTNodeResult::Failed;

  UAnimInstance* AnimInstance = AICharacter->GetMesh()->GetAnimInstance();
  if (!AnimInstance) return EBTNodeResult::Failed;

  AnimInstance->Montage_Play(MontageToPlay);

  return EBTNodeResult::Succeeded;
}