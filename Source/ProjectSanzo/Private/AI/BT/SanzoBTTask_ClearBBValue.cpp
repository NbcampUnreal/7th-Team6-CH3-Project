#include "AI/BT/SanzoBTTask_ClearBBValue.h"
#include "BehaviorTree/BlackboardComponent.h"

USanzoBTTask_ClearBBValue::USanzoBTTask_ClearBBValue()
{
  NodeName = TEXT("Clear Blackboard Value");
}

EBTNodeResult::Type USanzoBTTask_ClearBBValue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
  UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

  if (BlackboardComp)
  {
    BlackboardComp->ClearValue(BlackboardKey.SelectedKeyName);

    return EBTNodeResult::Succeeded;
  }

  return EBTNodeResult::Failed;
}
