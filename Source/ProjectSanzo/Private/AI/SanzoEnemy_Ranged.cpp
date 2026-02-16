#include "AI/SanzoEnemy_Ranged.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"
#include "Engine/DamageEvents.h"
#include "Components/SkeletalMeshComponent.h" 

ASanzoEnemy_Ranged::ASanzoEnemy_Ranged()
{
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

  if (WeaponMesh->DoesSocketExist(TEXT("Muzzle")))
  {
    TraceStart = WeaponMesh->GetSocketLocation(TEXT("Muzzle"));
  }
  else
  {
    TraceStart = GetActorLocation() + (GetActorForwardVector() * 50.0f) + FVector(0.f, 0.f, 50.f);
  }

  FVector TraceEnd = TraceStart + (TraceRotation.Vector() * AttackRange);

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

    // 피격 위치에 피 이펙트 생성
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