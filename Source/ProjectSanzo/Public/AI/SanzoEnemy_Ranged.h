#pragma once

#include "CoreMinimal.h"
#include "AI/SanzoEnemyBase.h"
#include "SanzoEnemy_Ranged.generated.h"

UCLASS()
class PROJECTSANZO_API ASanzoEnemy_Ranged : public ASanzoEnemyBase
{
  GENERATED_BODY()

public:
  ASanzoEnemy_Ranged();

  virtual void Attack() override;

  UFUNCTION(BlueprintCallable, Category = "Combat")
  void FireHitScan();

protected:
  // 원거리 적 전용 공격력
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
  float ShootDamage = 20.f;

  // Visual/Sound Effects
  UPROPERTY(EditAnywhere, Category = "Effects|Visual")
  TObjectPtr<class UParticleSystem> MuzzleFlashEffect;

  UPROPERTY(EditAnywhere, Category = "Effects|Visual")
  TObjectPtr<class UParticleSystem> HitEffect;

  UPROPERTY(EditAnywhere, Category = "Effects|Sound")
  TObjectPtr<class USoundBase> FireSound;

  // 디버그 라인 표시 여부
  UPROPERTY(EditAnywhere, Category = "Combat|Debug")
  bool bShowDebugTrace = true;
};
