#pragma once

#include "CoreMinimal.h"
#include "Items/SanzoItemBase.h"
#include "SanzoItem_Health.generated.h"

#pragma region ItemHealth
UCLASS()
class PROJECTSANZO_API ASanzoItem_Health : public ASanzoItemBase
{
  GENERATED_BODY()
public:
  ASanzoItem_Health();

  UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Item")
  int32 HealAmount;

  virtual void ActivateItem(AActor* Activator) override;
};
#pragma endregion 최윤서