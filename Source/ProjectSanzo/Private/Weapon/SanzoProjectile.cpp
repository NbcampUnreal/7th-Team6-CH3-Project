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
	// 기본 콜리전 설정
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	// 어딘가에 충돌할때마다 OnHit 함수 실행
	CollisionComp->SetNotifyRigidBodyCollision(true);
	// 커스텀 Collision 설정을 위해 모든 충돌 무시로 설정
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	// Collision 설정
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);  // 벽, 바닥같이 움직이지 않는 것들
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block); // 움직이는 물체
	CollisionComp->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Block);  // 적 스켈레탈 메시
	CollisionComp->SetCollisionResponseToChannel(ECC_Destructible, ECR_Block); // 파괴되는 물체도 블락하도록 추가
	CollisionComp->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Ignore);
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
	// 화살의 콜리전 컴포넌트를 따라다니도록 부착
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
		WeaponOwner->ApplyDamageToTarget(OtherActor, Hit,ArrowDamage);
		WeaponOwner->PlayImpactEffects(Hit);
	}

	// 맞은 화살 붙이는 로직

	// 어딘가에 맞으면 정지
	ProjectileMovement->StopMovementImmediately();
	// 정지 후 화살의 콜리전 판정 없애기
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// 맞은게 적이라면 맞고 움직일때 화살도 같이 움직이게 수정
	AttachToComponent(OtherComp, FAttachmentTransformRules::KeepWorldTransform, Hit.BoneName);
	// 2초 후에 사라짐
	SetLifeSpan(2.0f);
}

void ASanzoProjectile::SetArrowSpeed(float NewSpeed)
{
	// 기존의 최대 속도를 새로받은 속도로 바꾸기
	ProjectileMovement->MaxSpeed = NewSpeed;
	ProjectileMovement->InitialSpeed = NewSpeed;

	// 원래 화살에 적용되어있던 속도 새 속도로 바꾸기
	ProjectileMovement->Velocity = ProjectileMovement->Velocity.GetSafeNormal() * NewSpeed;
}

void ASanzoProjectile::SetArrowDamage(float NewDamage)
{
	ArrowDamage = NewDamage;
}

void ASanzoProjectile::BeginPlay()
{
	Super::BeginPlay();

	// 쏜 플레이어 충돌 무시
	if (GetInstigator())
	{
		CollisionComp->IgnoreActorWhenMoving(GetInstigator(), true);
	}

	// 소환된 활 충돌 무시
	if (GetOwner())
	{
		CollisionComp->IgnoreActorWhenMoving(GetOwner(), true);
	}
}
