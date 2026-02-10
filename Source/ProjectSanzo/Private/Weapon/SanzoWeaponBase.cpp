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
  MaxAmmo = 30;
  MaxRange = 5000.0f;
  CurrentAmmo = 30;
  bInfiniteAmmo = false;
  bIsReloading = false;
}

void ASanzoWeaponBase::StartFire()
{

}

void ASanzoWeaponBase::StopFire()
{

}

void ASanzoWeaponBase::Reload()
{
  if (bIsReloading || (CurrentAmmo >= MaxAmmo && !bInfiniteAmmo)) return;

  bIsReloading = true;

  if (WeaponMesh)
  {
    float ReloadDuration = 2.0f;

    GetWorldTimerManager().SetTimer(
      FireTimerHandle,
      this,
      &ASanzoWeaponBase::FinishReload,
      ReloadDuration,
      false
    );
  }
}

void ASanzoWeaponBase::FinishReload()
{
  bIsReloading = false;

  if (bInfiniteAmmo == false)
  {
    CurrentAmmo = MaxAmmo;
    UE_LOG(LogTemp, Log, TEXT("Reload Finished."));
  }
  else
  {
    UE_LOG(LogTemp, Log, TEXT("Reload Action Finished"));
  }
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

  APawn* OwnerPawn = Cast<APawn>(GetOwner());
  if (!OwnerPawn) return;

  AController* OwnerController = OwnerPawn->GetController();

  UGameplayStatics::ApplyDamage(
    TargetActor,
    BaseDamage,
    OwnerController,
    this,
    UDamageType::StaticClass()
  );
}



