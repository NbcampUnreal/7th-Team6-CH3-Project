#pragma once

#include "CoreMinimal.h"
#include "AI/SanzoEnemyBase.h"
#include "SanzoEnemy_Melee.generated.h"

UCLASS()
class PROJECTSANZO_API ASanzoEnemy_Melee : public ASanzoEnemyBase
{
  GENERATED_BODY()
public:
  ASanzoEnemy_Melee();

  // 검에 부착할 박스 콜리전
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
  TObjectPtr<class UBoxComponent> MeleeCollision;

  // 애니메이션 프레임에 맞춰 콜리전을 켜고 끌 함수
  UFUNCTION(BlueprintCallable, Category = "Combat")
  void EnableWeaponCollision();

  UFUNCTION(BlueprintCallable, Category = "Combat")
  void DisableWeaponCollision();

  // 플레이어가 칼에 닿았을 때 실행될 함수
  UFUNCTION()
  void OnMeleeOverlap(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult);

protected:
  // 근접 공격 데미지
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
  float MeleeDamage = 30.0f;
};
