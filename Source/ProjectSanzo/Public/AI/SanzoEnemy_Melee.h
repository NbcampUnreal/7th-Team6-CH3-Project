#pragma once

#include "CoreMinimal.h"
#include "AI/SanzoEnemy_MeleeBase.h"
#include "SanzoEnemy_Melee.generated.h"

UCLASS()
class PROJECTSANZO_API ASanzoEnemy_Melee : public ASanzoEnemy_MeleeBase
{
  GENERATED_BODY()
public:
  ASanzoEnemy_Melee();

  virtual void Attack() override;
};
