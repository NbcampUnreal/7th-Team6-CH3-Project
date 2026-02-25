#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "SanzoBTTask_ChoosePattern.generated.h"

UCLASS()
class PROJECTSANZO_API USanzoBTTask_ChoosePattern : public UBTTask_BlackboardBase
{
  GENERATED_BODY()
public:
  USanzoBTTask_ChoosePattern();

protected:
  virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
  // 2페이즈인지 확인하는 키
  UPROPERTY(EditAnywhere, Category = "Blackboard")
  struct FBlackboardKeySelector IsPhase2Key;

  // 플레이어와의 거리 키
  UPROPERTY(EditAnywhere, Category = "Blackboard")
  struct FBlackboardKeySelector DistanceKey;
};
