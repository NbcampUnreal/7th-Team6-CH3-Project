#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SanzoProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class PROJECTSANZO_API ASanzoProjectile : public AActor
{
  GENERATED_BODY()

public:
	ASanzoProjectile();

	// 화살 히트 판정(화살촉)
	UPROPERTY(VisibleAnywhere, Category = "Projectile")
	USphereComponent* CollisionComp;

	// 화살 메시
	UPROPERTY(VisibleAnywhere, Category = "Projectile")
	UStaticMeshComponent* MeshComp;

	// 물리움직임 담당
	UPROPERTY(VisibleAnywhere, Category = "Movement")
	UProjectileMovementComponent* ProjectileMovement;

	// 화살 궤적 이펙트 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UNiagaraComponent* TrailEffectComp;

	// 적이 맞았을 때 이펙트
	UPROPERTY(EditAnywhere, Category = "Effects")
	UNiagaraSystem* EnemyImpactEffect;

	// 활에서 받을 데미지
	float Damage;

	// 적이 맞았을 때 데미지 줄 함수
	UFUNCTION()
	void OnHit(
		UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit);

	// 차징에 따른 속도 변경 함수
	void SetArrowSpeed(float NewSpeed);
	// 화살마다 가지게 될 데미지
	float ArrowDamage;
	//가지게 될 데미지 설정 함수
	void SetArrowDamage(float NewDamage);

protected:
	virtual void BeginPlay() override;
};
