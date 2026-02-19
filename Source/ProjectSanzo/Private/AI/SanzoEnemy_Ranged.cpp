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

  FVector TraceStart = GetActorLocation();

  if (WeaponMesh->DoesSocketExist(TEXT("MuzzleFlash")))
  {
    TraceStart = WeaponMesh->GetSocketLocation(TEXT("MuzzleFlash"));
  }
  else
  {
    TraceStart = GetActorLocation() + (GetActorForwardVector() * 50.0f) + FVector(0.f, 0.f, 50.f);
  }

  FRotator TraceRotation = LockedAimRotation;
  FVector TraceEnd = TraceStart + (LockedAimRotation.Vector() * AttackRange);

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
      if (!HitActor->IsA<ASanzoEnemyBase>())
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

    ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (PlayerCharacter)
    {
      // 플레이어 위치 계산
      FVector PlayerCenter = PlayerCharacter->GetActorLocation() + FVector(0.f, 0.f, 30.f);

      // 매 프레임 총구에서 플레이어까지 가상의 선을 쏴서 벽이 있는지 검사
      FHitResult SightHit;
      FCollisionQueryParams Params;
      Params.AddIgnoredActor(this);

      bool bHitSomething = GetWorld()->LineTraceSingleByChannel(SightHit, TraceStart, PlayerCenter, ECC_Visibility, Params);

      if (bHitSomething)
      {
        AActor* HitActor = SightHit.GetActor();
        // 맞은 것이 플레이어나 동료 적이 아니라면
        if (HitActor && !HitActor->ActorHasTag("Player") && !HitActor->IsA<ASanzoEnemyBase>())
        {
          // 조준을 즉시 취소
          CancelAiming();
          return;
        }
      }

      if (!bIsAimLocked)
      {
        FVector DirectionToPlayer = (PlayerCenter - TraceStart).GetSafeNormal();
        FRotator TargetRot = DirectionToPlayer.Rotation();

        // 적 몸통(Yaw)을 플레이어 쪽으로 자연스럽게 회전
        FRotator NewRot = GetActorRotation();
        NewRot.Yaw = FMath::RInterpTo(GetActorRotation(), TargetRot, DeltaTime, 8.0f).Yaw;
        SetActorRotation(NewRot);

        // 에임 오프셋용 상하 각도(Pitch) 자연스럽게 적용
        float TargetPitch = FRotator::NormalizeAxis(TargetRot.Pitch);
        AimPitch = FMath::FInterpTo(AimPitch, TargetPitch, DeltaTime, 15.0f);

        // 나중에 쏠 방향과 레이저 끝점 갱신 (추적 중)
        LockedAimRotation = TargetRot;
        LockedTraceEnd = TraceStart + (DirectionToPlayer * AttackRange);
      }

      // 점멸 타이밍이 되면 조준을 락(Lock)
      if (CurrentAimTime >= BlinkStartTime)
      {
        bIsAimLocked = true;
      }
    }


    // 조준선 그리기 설정
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
      DrawDebugLine(
        GetWorld(),
        TraceStart,
        LockedTraceEnd,
        LaserColor,
        false,
        0.0f,
        0,
        LineThickness
      );
    }
  }
  else
  {
    AimPitch = FMath::FInterpTo(AimPitch, 0.f, DeltaTime, 10.0f);
  }
}

void ASanzoEnemy_Ranged::StartAiming()
{
  bIsAiming = true;
  CurrentAimTime = 0.f;
  bIsAimLocked = false;

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

void ASanzoEnemy_Ranged::CancelAiming()
{
  bIsAiming = false;
  bIsAimLocked = false;
  CurrentAimTime = 0.f;

  GetWorldTimerManager().ClearTimer(AimTimerHandle);

  if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
  {
    AnimInstance->Montage_Stop(0.2f, AttackMontage);
  }
}
