#pragma once

#include "CoreMinimal.h"
#include "Items/SanzoItemBase.h"
#include "SanzoItem_Ammo.generated.h"

#pragma region ItemAmmo
UCLASS()
class PROJECTSANZO_API ASanzoItem_Ammo : public ASanzoItemBase
{
  GENERATED_BODY()
public:
  ASanzoItem_Ammo();

  UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Item")
  int32 AmmoAmount;
  
  virtual void ActivateItem(AActor* Activator) override;
};
#pragma endregion 최윤서