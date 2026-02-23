#include "Weapon/SanzoProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "AI/SanzoEnemyBase.h"
#include "Weapon/SanzoWeaponBase.h"
#include "NiagaraComponent.h"

ASanzoProjectile::ASanzoProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	// 루트 컴포넌트 설정
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ASanzoProjectile::OnHit);
	RootComponent = CollisionComp;

	// 화살 외형 담당
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(CollisionComp);
	// 콜리전 없음으로 설정(콜리전을 담당하는 CollisionComp와 겹쳐있어서)
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 투사체 움직임 담당
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.0f;
	ProjectileMovement->MaxSpeed = 3000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true; // 날아가는 방향으로 회전함
	ProjectileMovement->bShouldBounce = false; // 화살은 어딘가 맞았을 때 튕기지 않음
	InitialLifeSpan = 3.0f; // 아무것도 못맞추면 3초 후 사라짐

	// 화살의 트레일 이펙트 담당
	TrailEffectComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailEffectComp"));
	// 화살의 꼬리(루트)를 따라다니도록 부착
	TrailEffectComp->SetupAttachment(RootComponent);

}

// 무언가에 맞았을 때
void ASanzoProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 콜리전과 겹쳐진게 쓰레기값, 이 화살, 쏜 사람, 쏜 무기 라면 이 함수 실행안함.
	if (OtherActor == nullptr || OtherActor == this || OtherActor == GetInstigator() || OtherActor == GetOwner()) { return; }

	// 이 화살의 주인(활) 캐스팅
	ASanzoWeaponBase* WeaponOwner = Cast<ASanzoWeaponBase>(GetOwner());

	// 활이 정상적으로 있다면
	if (WeaponOwner)
	{
		// 웨폰베이스의 데미지 적용 함수 실행
		WeaponOwner->ApplyDamageToTarget(OtherActor, Hit);
	}

	// 화살 파괴(후에 수정할수도 있음)
	Destroy();
}
