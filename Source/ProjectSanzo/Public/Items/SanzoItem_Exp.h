#pragma once

#include "CoreMinimal.h"
#include "Items/SanzoItemBase.h"
#include "SanzoItem_Exp.generated.h"

#pragma region ItemExp
UCLASS()
class PROJECTSANZO_API ASanzoItem_Exp : public ASanzoItemBase
{
	GENERATED_BODY()
public:
	ASanzoItem_Exp();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Item")
	int32 ExpAmount;
  virtual void ActivateItem(AActor* Activator) override;
};
#pragma endregion 최윤서