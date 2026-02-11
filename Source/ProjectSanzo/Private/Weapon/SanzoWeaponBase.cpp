#include "Weapon/SanzoWeaponBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"



ASanzoWeaponBase::ASanzoWeaponBase()
{
  PrimaryActorTick.bCanEverTick = false;

  WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
  SetRootComponent(WeaponMesh);

  WeaponMesh->SetCollisionProfileName(TEXT("NoCollision"));

  FireStartLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("FireStartLocation"));
  FireStartLocation->SetupAttachment(WeaponMesh);

  BaseDamage = 10.0f;
  FireRate = 0.1f;
  MaxRange = 5000.0f;
  CurrentAmmo = 30;
  bInfiniteAmmo = false;
}

void ASanzoWeaponBase::StartFire()
{

}

void ASanzoWeaponBase::StopFire()
{

}

void ASanzoWeaponBase::PlayFireEffects()
{
  ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
  if (OwnerCharacter)
  {
    UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
    if (AnimInstance && CharacterFireMontage)
    {
      AnimInstance->Montage_Play(CharacterFireMontage, 1.0f);
    }
  }

  if (WeaponMesh && WeaponFireAnim)
  {
    WeaponMesh->PlayAnimation(WeaponFireAnim, false);
  }
}

void ASanzoWeaponBase::ApplyDamageToTarget(AActor* TargetActor, FHitResult HitInfo)
{
  if (!TargetActor) return;

  float FinalDamage = BaseDamage;
  // 헤드샷 판정되는지 확인용
  bool bIsHeadshot = false;

  if (HitInfo.BoneName == HeadBoneName)
  {
    FinalDamage *= HeadshotMultiplier;
    bIsHeadshot = true;
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

  // 로그 체크용 후에 확인되면 삭제 가능
  if (bIsHeadshot)
  {
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("HEADSHOT! Damage: %f"), FinalDamage));
  }
  else
  {
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("Body Hit. Damage: %f"), FinalDamage));
  }
}



