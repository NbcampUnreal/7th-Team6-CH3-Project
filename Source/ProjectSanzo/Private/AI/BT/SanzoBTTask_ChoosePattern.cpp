#include "AI/BT/SanzoBTTask_ChoosePattern.h"
#include "BehaviorTree/BlackboardComponent.h"

USanzoBTTask_ChoosePattern::USanzoBTTask_ChoosePattern()
{
  NodeName = TEXT("Choose Boss Pattern");
}

EBTNodeResult::Type USanzoBTTask_ChoosePattern::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
  UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
  if (!BlackboardComp) return EBTNodeResult::Failed;

  bool bIsPhase2 = BlackboardComp->GetValueAsBool(IsPhase2Key.SelectedKeyName);
  float DistanceToTarget = BlackboardComp->GetValueAsFloat(DistanceKey.SelectedKeyName);

  int32 SelectedPattern = 1;

  // 거리가 멀면
  if (DistanceToTarget > 600.0f)
  {
    // 1페이즈: 일섬(2) 고정으로 빠르게 접근
    // 2페이즈: 일섬(2) 또는 검기 방출(4) 중 하나
    SelectedPattern = (bIsPhase2) ? (FMath::RandBool() ? 2 : 4) : 2;
  }
  // 거리가 가까우면
  else
  {
    // 2페이즈: 1(3연격), 3(내려찍기), 5(난무)
    if (bIsPhase2)
    {
      int32 RandomChoice = FMath::RandRange(1, 3);
      SelectedPattern = (RandomChoice == 1) ? 1 : ((RandomChoice == 2) ? 3 : 5);
    }
    // 1페이즈: 1(3연격) 또는 3(내려찍기) 중 하나
    else
    {
      SelectedPattern = FMath::RandBool() ? 1 : 3;
    }
  }

  if (ForcePattern > 0)
  {
    SelectedPattern = ForcePattern;
  }

  BlackboardComp->SetValueAsInt(BlackboardKey.SelectedKeyName, SelectedPattern);

  return EBTNodeResult::Succeeded;
}