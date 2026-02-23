#include "AI/SanzoEnemy_Melee.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

ASanzoEnemy_Melee::ASanzoEnemy_Melee()
{
  // 근접 적 기본 스탯 세팅
  MaxHP = 120.f;
  AttackRange = 180.f;

  // 근접 공격용 박스 콜리전 생성
  MeleeCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("MeleeCollision"));

  // 무기에 콜리전을 부착
  MeleeCollision->SetupAttachment(StaticWeaponMesh);

  // 칼을 휘두를 때만 충돌 판정이 활성화되도록 초기에는 비활성화
  MeleeCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
  MeleeCollision->SetCollisionObjectType(ECC_WorldDynamic);
  MeleeCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
  MeleeCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

  // 오버랩 이벤트 연결
  MeleeCollision->OnComponentBeginOverlap.AddDynamic(this, &ASanzoEnemy_Melee::OnMeleeOverlap);
}

void ASanzoEnemy_Melee::EnableWeaponCollision()
{
  // 공격 판정 시작
  MeleeCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ASanzoEnemy_Melee::DisableWeaponCollision()
{
  // 공격 판정 종료
  MeleeCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASanzoEnemy_Melee::OnMeleeOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
  if (OtherActor && OtherActor != this && OtherActor->ActorHasTag("Player"))
  {
    UGameplayStatics::ApplyDamage(
      OtherActor,
      MeleeDamage,
      GetController(),
      this,
      UDamageType::StaticClass()
    );

    // 한 번 때리면 콜리전을 즉시 비활성화 
    DisableWeaponCollision();
  }
}