#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SanzoBTTask_PlayMontage.generated.h"

UCLASS()
class PROJECTSANZO_API USanzoBTTask_PlayMontage : public UBTTaskNode
{
  GENERATED_BODY()

public:
  USanzoBTTask_PlayMontage();

protected:
  virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

  UPROPERTY(EditAnywhere, Category = "Animation")
  UAnimMontage* MontageToPlay;
};
