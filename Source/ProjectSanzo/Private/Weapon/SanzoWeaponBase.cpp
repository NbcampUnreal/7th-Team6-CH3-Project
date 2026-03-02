#include "Weapon/SanzoWeaponBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "AI/SanzoEnemyBase.h"
#include "Components/DecalComponent.h"
#include "NiagaraFunctionLibrary.h"




ASanzoWeaponBase::ASanzoWeaponBase()
{
  PrimaryActorTick.bCanEverTick = false;

  WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
  SetRootComponent(WeaponMesh);

  WeaponMesh->SetCollisionProfileName(TEXT("NoCollision"));

  FireStartLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("FireStartLocation"));
  FireStartLocation->SetupAttachment(WeaponMesh);

  BaseDamage = 10.0f;
  CurrentAmmo = 30;
  bInfiniteAmmo = false;

  AttachSocketName = TEXT("HandGrip_R");
}

void ASanzoWeaponBase::StartFire()
{

}

void ASanzoWeaponBase::StopFire()
{

}

// 발사할 때 필요한 이펙트
void ASanzoWeaponBase::PlayFireEffects()
{
  ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
  if (OwnerCharacter)
  {
    UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
    if (AnimInstance && CharacterFireMontage)
    {
      AnimInstance->Montage_Play(CharacterFireMontage, 1.0f);

      //김형백
      
      AnimInstance->Montage_SetBlendedInDelegate(StartBowAttackDelegate, CharacterFireMontage);
      
    }
  }

  if (WeaponMesh && WeaponFireAnim)
  {
    WeaponMesh->PlayAnimation(WeaponFireAnim, false);
  }
}

// 맞은 대상이 출력해야할 이펙트
void ASanzoWeaponBase::PlayImpactEffects(FHitResult HitInfo)
{
  ASanzoEnemyBase* HitEnemy = Cast<ASanzoEnemyBase>(HitInfo.GetActor());

  // 맞춘 것이 적일 때
  if (HitEnemy)
  {
    if (BloodSplatterEffect)
    {
      UNiagaraFunctionLibrary::SpawnSystemAtLocation(
        GetWorld(),
        BloodSplatterEffect,
        HitInfo.ImpactPoint,
        HitInfo.ImpactNormal.Rotation()
      );
    }

    //김형백 제작 히트시 소리
    UGameplayStatics::PlaySound2D(GetWorld(), HitSuccessSound);

  }
  // 맞춘 것이 지형지물일 때
  else
  {
    if (ImpactEffect)
    {


      // 트레이스와 액터가 만난 위치에 파티클 이펙트 생성
      UGameplayStatics::SpawnEmitterAtLocation(
        GetWorld(),
        ImpactEffect,
        HitInfo.ImpactPoint,
        HitInfo.ImpactNormal.Rotation()
      );
    }

    if (BulletHoleDecal)
    {
      // 데칼이 벽을 향하도록 회전값 설정
      FRotator DecalRotation = HitInfo.ImpactNormal.Rotation();

      UDecalComponent* SpawnDecal = UGameplayStatics::SpawnDecalAtLocation(
        GetWorld(),
        BulletHoleDecal,
        FVector(DecalSize, DecalSize, DecalSize), // 데칼의 크기 (X, Y, Z)
        HitInfo.ImpactPoint,       // 맞은 위치
        DecalRotation,             // 벽에 수직으로 붙는 각도
        4.0f                       // 유지 시간
      );

      if (SpawnDecal)
      {
        // 총알 자국이 보이는 가시 거리 늘리는 용도
        SpawnDecal->SetFadeScreenSize(0.001f);
      }

    }
  }
}

void ASanzoWeaponBase::ApplyWeaponStatUpgrade(EUpgradeType Type, float Value)
{
  switch (Type)
  {
  case EUpgradeType::Damage:
    BaseDamage = BaseDamage * (1 + Value);
    break;
  default:
    GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("샤갈! 이상한값이 발생했어요!"));
    break;
  }
}

void ASanzoWeaponBase::ApplyDamageToTarget(AActor* TargetActor, FHitResult HitInfo, float DamageToApply)
{
  if (!TargetActor) return;

  // 맞은 대상이 SanzoEnemyBase 클래스인지 확인용
  ASanzoEnemyBase* HitEnemy = Cast<ASanzoEnemyBase>(TargetActor);
  float FinalDamage = DamageToApply;

  // 맞은 액터가 SanzoEnemyBase가 맞다면 데미지 적용
  if (HitEnemy)
  {
  	//적 피격 HUD 이펙트 재생용 방송 - 작업자: 이준로 
  	OnEnemyHit.Broadcast();

    if (HitInfo.BoneName == HeadBoneName)
    {
      FinalDamage *= HeadshotMultiplier;
    }

    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (!OwnerPawn) return;

    AController* OwnerController = OwnerPawn->GetController();

    UGameplayStatics::ApplyDamage(
      TargetActor,
      FinalDamage,
      OwnerController,
      this,
      UDamageType::StaticClass()
    );
  }
#pragma region DestructibleItem
  // 부숴지는 물체 태그 확인
  else if (TargetActor->ActorHasTag(FName("Destructible")))
  {
    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    AController* OwnerController = OwnerPawn ? OwnerPawn->GetController() : nullptr;
    // 데미지 전달
    UGameplayStatics::ApplyDamage(TargetActor, FinalDamage, OwnerController, this, UDamageType::StaticClass());
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("WB: Hit Destructible Object"));
  }
#pragma endregion 최윤서

  if (HitEnemy)
  {
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Hit Enemy"));
  }
}