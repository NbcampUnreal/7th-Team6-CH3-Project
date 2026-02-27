#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SanzoEnemy_Boss_SwordAura.generated.h"

UCLASS()
class PROJECTSANZO_API ASanzoEnemy_Boss_SwordAura : public AActor
{
  GENERATED_BODY()

public:
  ASanzoEnemy_Boss_SwordAura();

protected:
  virtual void BeginPlay() override;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
  TObjectPtr<class UBoxComponent> CollisionBox;

  // 검기 피격 시 비율 데미지
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
  float AuraPercentDamage = 0.3f;

  // 검기 시각 효과
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
  TObjectPtr<class UStaticMeshComponent> AuraMesh;

  // 투사체 컴포넌트
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
  TObjectPtr<class UProjectileMovementComponent> ProjectileMovement;

  // 충돌했을 때 실행될 함수
  UFUNCTION()
  void OnAuraOverlap(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult
  );
};
