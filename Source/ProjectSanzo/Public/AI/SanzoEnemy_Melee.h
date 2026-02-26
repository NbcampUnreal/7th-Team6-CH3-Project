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

protected:
#pragma region Sound
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects|Sound")
  USoundBase* MeleeAttackSound;
#pragma endregion 최윤서
};
