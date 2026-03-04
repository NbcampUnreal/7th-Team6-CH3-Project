#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SanzoEnemyInterface.generated.h"

UINTERFACE(MinimalAPI)
class USanzoEnemyInterface : public UInterface
{
  GENERATED_BODY()
};

class PROJECTSANZO_API ISanzoEnemyInterface
{
  GENERATED_BODY()

public:
  virtual void Attack() = 0;
  virtual bool IsDead() const = 0;
  virtual float GetAttackRange() const = 0;
};
