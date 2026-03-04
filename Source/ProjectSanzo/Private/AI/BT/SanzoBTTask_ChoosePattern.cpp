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

  int32 LastPattern = BlackboardComp->GetValueAsInt(BlackboardKey.SelectedKeyName);
  int32 SelectedPattern = 1;

  // ==========================================
  // 원거리 패턴
  // ==========================================
  if (DistanceToTarget > 600.0f)
  {
    if (bIsPhase2)
    {
      // 2페이즈: 2번(일섬) 70%, 4번(검기) 30% 확률로 실행
      SelectedPattern = (FMath::FRand() < 0.7f) ? 2 : 4;
    }
    else
    {
      // 1페이즈: 무조건 2번(일섬)으로 빠르게 접근
      SelectedPattern = 2;
    }
  }
  // ==========================================
  // 근접 패턴
  // ==========================================
  else
  {
    TArray<int32> AvailablePatterns;

    if (bIsPhase2)
    {
      // 2페이즈: 3(내려찍기), 5(난무)
      AvailablePatterns = { 3, 5 };
    }
    else
    {
      // 1페이즈: 1(3연격), 3(내려찍기)
      AvailablePatterns = { 1, 3 };
    }

    // 똑같은 패턴이 연속으로 나오는 것을 방지
    if (AvailablePatterns.Contains(LastPattern))
    {
      AvailablePatterns.RemoveSingle(LastPattern);
    }

    // 남은 패턴 중에서 랜덤하게 하나 선택
    if (AvailablePatterns.Num() > 0)
    {
      int32 RandomIndex = FMath::RandRange(0, AvailablePatterns.Num() - 1);
      SelectedPattern = AvailablePatterns[RandomIndex];
    }
    else
    {
      // 만약 배열이 비었다면 기본값 강제 할당
      SelectedPattern = bIsPhase2 ? 5 : 1;
    }
  }

  // 디버그용 강제 패턴
  if (ForcePattern > 0)
  {
    SelectedPattern = ForcePattern;
  }

  BlackboardComp->SetValueAsInt(BlackboardKey.SelectedKeyName, SelectedPattern);

  return EBTNodeResult::Succeeded;
}