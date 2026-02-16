#include "AI/SanzoEnemy_Ranged.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"
#include "Engine/DamageEvents.h"
#include "Components/SkeletalMeshComponent.h" 
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h" 
#include "TimerManager.h"

ASanzoEnemy_Ranged::ASanzoEnemy_Ranged()
{
  PrimaryActorTick.bCanEverTick = true;
  MaxHP = 80.f;
  AttackRange = 2500.f;
}

void ASanzoEnemy_Ranged::Attack()
{
  Super::Attack();
}

void ASanzoEnemy_Ranged::FireHitScan()
{
  if (IsDead() || !WeaponMesh) return;

  // 발사 시작 위치와 방향 설정
  FVector TraceStart = GetActorLocation();
  FRotator TraceRotation = GetActorRotation();

  if (WeaponMesh->DoesSocketExist(TEXT("MuzzleFlash")))
  {
    TraceStart = WeaponMesh->GetSocketLocation(TEXT("MuzzleFlash"));
  }
  else
  {
    TraceStart = GetActorLocation() + (GetActorForwardVector() * 50.0f) + FVector(0.f, 0.f, 50.f);
  }

  FRotator AimRotation = GetActorRotation();
  AimRotation.Pitch = LockedAimPitch;
  FVector TraceEnd = TraceStart + (AimRotation.Vector() * AttackRange);

  // 발사 이펙트 및 사운드 재생
  if (MuzzleFlashEffect)
  {
    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlashEffect, TraceStart, TraceRotation);
  }
  if (FireSound)
  {
    UGameplayStatics::PlaySoundAtLocation(this, FireSound, TraceStart);
  }

  // 히트스캔 (LineTrace)
  FHitResult HitResult;
  FCollisionQueryParams QueryParams;
  QueryParams.AddIgnoredActor(this);

  bool bHit = GetWorld()->LineTraceSingleByChannel(
    HitResult,
    TraceStart,
    TraceEnd,
    ECC_Visibility,
    QueryParams
  );

  FVector TraceEndPoint = bHit ? HitResult.ImpactPoint : TraceEnd;

  // 궤적 이펙트 생성
  if (TracerEffect)
  {
    FRotator TracerRotation = (TraceEndPoint - TraceStart).Rotation();
    UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, TraceStart, TracerRotation, true);

    if (TracerComp)
    {
      TracerComp->SetVectorParameter(FName("Target"), TraceEndPoint);
    }
  }

  // 피격 처리
  if (bHit)
  {
    AActor* HitActor = HitResult.GetActor();
    if (HitActor)
    {
      UGameplayStatics::ApplyDamage(
        HitActor,
        ShootDamage,
        GetController(),
        this,
        UDamageType::StaticClass()
      );
    }

    // 피격 위치에 이펙트 생성
    if (HitEffect)
    {
      UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEffect, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
    }

    // [디버그용] 명중 시 초록색 선 출력
    if (bShowDebugTrace)
    {
      DrawDebugLine(GetWorld(), TraceStart, HitResult.ImpactPoint, FColor::Green, false, 2.0f, 0, 1.0f);
    }
  }
  else
  {
    // [디버그용] 허공에 쏘면 빨간색 선 출력
    if (bShowDebugTrace)
    {
      DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 2.0f, 0, 1.0f);
    }
  }
}

void ASanzoEnemy_Ranged::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

  if (bIsAiming && !IsDead() && WeaponMesh)
  {
    CurrentAimTime += DeltaTime;

    FVector TraceStart = GetActorLocation();
    if (WeaponMesh->DoesSocketExist(TEXT("MuzzleFlash")))
    {
      TraceStart = WeaponMesh->GetSocketLocation(TEXT("MuzzleFlash"));
    }

    FRotator AimRotation = GetActorRotation();
    AimRotation.Pitch = LockedAimPitch;

    FVector TargetLocation = TraceStart + (AimRotation.Vector() * AttackRange);

    bool bDrawLaser = true;
    float LineThickness = 2.0f;
    FColor LaserColor = FColor::Yellow;

    if (CurrentAimTime >= BlinkStartTime)
    {
      if (FMath::Fmod(CurrentAimTime, 0.2f) < 0.1f) { bDrawLaser = false; }
      LineThickness = 8.0f;
      LaserColor = FColor::Red;
    }
    else
    {
      float Alpha = CurrentAimTime / BlinkStartTime;
      LineThickness = FMath::Lerp(1.0f, 6.0f, Alpha);
      uint8 GreenValue = FMath::Lerp(255.0f, 0.0f, Alpha);
      LaserColor = FColor(255, GreenValue, 0);
    }

    if (bDrawLaser)
    {
      DrawDebugLine(GetWorld(), TraceStart, TargetLocation, LaserColor, false, 0.0f, 0, LineThickness);
    }
  }
}
void ASanzoEnemy_Ranged::StartAiming()
{
  bIsAiming = true;
  CurrentAimTime = 0.f;

  FVector TraceStart = GetActorLocation();
  if (WeaponMesh && WeaponMesh->DoesSocketExist(TEXT("MuzzleFlash")))
  {
    TraceStart = WeaponMesh->GetSocketLocation(TEXT("MuzzleFlash"));
  }
  ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
  if (PlayerCharacter)
  {
    FVector PlayerCenter = PlayerCharacter->GetActorLocation() + FVector(0.f, 0.f, 30.f);
    FVector DirectionToPlayer = (PlayerCenter - TraceStart).GetSafeNormal();
    LockedAimPitch = DirectionToPlayer.Rotation().Pitch;
  }
  else
  {
    LockedAimPitch = GetActorRotation().Pitch;
  }

  if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
  {
    if (AttackMontage)
    {
      AnimInstance->Montage_SetPlayRate(AttackMontage, 0.0f);
    }
  }
  GetWorldTimerManager().SetTimer(AimTimerHandle, this, &ASanzoEnemy_Ranged::ResumeAiming, TotalAimTime, false);
}

void ASanzoEnemy_Ranged::ResumeAiming()
{
  if (IsDead()) return;

  if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
  {
    if (AttackMontage)
    {
      AnimInstance->Montage_SetPlayRate(AttackMontage, 1.0f);
    }
  }
}

void ASanzoEnemy_Ranged::StopAiming()
{
  bIsAiming = false;
  CurrentAimTime = 0.f;

  GetWorldTimerManager().ClearTimer(AimTimerHandle);
}