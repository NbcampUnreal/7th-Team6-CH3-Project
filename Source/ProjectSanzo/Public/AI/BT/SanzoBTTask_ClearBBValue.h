#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "SanzoBTTask_ClearBBValue.generated.h"

UCLASS()
class PROJECTSANZO_API USanzoBTTask_ClearBBValue : public UBTTask_BlackboardBase
{
  GENERATED_BODY()
public:
  USanzoBTTask_ClearBBValue();

protected:
  virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
