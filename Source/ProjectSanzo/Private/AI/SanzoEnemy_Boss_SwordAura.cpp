#include "AI/SanzoEnemy_Boss_SwordAura.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Common/SanzoDamageType_Percent.h"

ASanzoEnemy_Boss_SwordAura::ASanzoEnemy_Boss_SwordAura()
{
  PrimaryActorTick.bCanEverTick = false;

  // 박스 콜리전 세팅
  CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
  RootComponent = CollisionBox;
  CollisionBox->SetCollisionProfileName(TEXT("Custom"));
  CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
  CollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
  CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
  CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
  CollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);

  // 검기 메시 세팅
  AuraMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AuraMesh"));
  AuraMesh->SetupAttachment(RootComponent);
  AuraMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

  // 발사체 무브먼트 세팅
  ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
  // 검기 날아가는 속도
  ProjectileMovement->InitialSpeed = 1500.f;
  ProjectileMovement->MaxSpeed = 2000.f;
  ProjectileMovement->ProjectileGravityScale = 0.f;
  // 날아가는 방향으로 자동 회전
  ProjectileMovement->bRotationFollowsVelocity = true;
  // 중력 무시
  ProjectileMovement->ProjectileGravityScale = 0.f;

  // 3초 뒤에 허공에서 자동 소멸
  InitialLifeSpan = 3.0f;
}

void ASanzoEnemy_Boss_SwordAura::BeginPlay()
{
	Super::BeginPlay();
  CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ASanzoEnemy_Boss_SwordAura::OnAuraOverlap);
}

void ASanzoEnemy_Boss_SwordAura::OnAuraOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
  if (!OtherActor || OtherActor == this || OtherActor == GetInstigator()) return;

  ACharacter* HitCharacter = Cast<ACharacter>(OtherActor);

  if (HitCharacter && HitCharacter->ActorHasTag("Player"))
  {
    UGameplayStatics::ApplyDamage(
      HitCharacter,
      AuraPercentDamage,
      GetInstigatorController(),
      this,
      USanzoDamageType_Percent::StaticClass()
    );

    // To-Do: 이펙트&사운드

    // 맞췄으니 검기는 소멸
    Destroy();
  }
}
