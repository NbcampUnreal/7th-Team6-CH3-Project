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
#include "GameFramework/CharacterMovementComponent.h"
#include "Common/SanzoGameplayTag.h"
#include "Common/SanzoLog.h"
#include "AI/Components/SanzoEnemyStunComponent.h" 
#include "GameplayTagAssetInterface.h"

ASanzoEnemy_Ranged::ASanzoEnemy_Ranged()
{
  PrimaryActorTick.bCanEverTick = true;
  MaxHP = 80.f;
  AttackRange = 2500.f;
  Exp = 20.f;
}

void ASanzoEnemy_Ranged::BeginPlay()
{
  Super::BeginPlay();
  CachedPlayer = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
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
    if (HitActor && !HitActor->IsA<ASanzoEnemyBase>())
    {
      UGameplayStatics::ApplyDamage(
        HitActor,
        ShootDamage,
        GetController(),
        this,
        UDamageType::StaticClass()
      );

      // 피격 위치에 이펙트 생성
      if (HitEffect)
      {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEffect, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
      }

#pragma region NotifyParried
      if (ACharacter* Character = Cast<ACharacter>(HitResult.GetActor()))
      {
        IGameplayTagAssetInterface* TagCheck = Cast<IGameplayTagAssetInterface>(Character);
        if (TagCheck->HasMatchingGameplayTag(SanzoTags::ParryWindow))
        {
          StunComponent->NotifyParried();
        }
      }
#pragma endregion 김형백

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

  if (bIsAiming && !IsDead() && WeaponMesh && CachedPlayer)
  {
    if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
    {
      // 현재 AttackMontage가 재생 중이 아니라면
      if (!AnimInstance->Montage_IsPlaying(AttackMontage))
      {
        // 즉시 조준 상태를 초기화
        CancelAimingAnim();
        return;
      }
    }

    CurrentAimTime += DeltaTime;

    static const FName MuzzleSocketName = TEXT("MuzzleFlash");
    static const FName SpineSocketName = TEXT("spine_04");
    static const FName HeadSocketName = TEXT("head");

    FVector TraceStart = GetActorLocation();
    if (WeaponMesh->DoesSocketExist(MuzzleSocketName))
    {
      TraceStart = WeaponMesh->GetSocketLocation(MuzzleSocketName);
    }

    // 플레이어 위치 계산
    FVector PlayerCenter = CachedPlayer->GetMesh()->GetSocketLocation(SpineSocketName);
    FVector EyeLocation = GetMesh()->GetSocketLocation(HeadSocketName);

    VisibilityCheckTimer += DeltaTime;
    if (VisibilityCheckTimer >= 0.1f)
    {
      VisibilityCheckTimer = 0.0f; // 타이머 초기화

      auto IsBlockedByEnvironment = [&](const FHitResult& Hit) -> bool
        {
          AActor* HitActor = Hit.GetActor();
          return HitActor && !HitActor->ActorHasTag("Player") && !HitActor->IsA<ASanzoEnemyBase>();
        };

      FCollisionQueryParams Params;
      Params.AddIgnoredActor(this);
      FHitResult SightHit;

      // 눈 기준 장애물 검사
      bool bHitEye = GetWorld()->LineTraceSingleByChannel(SightHit, EyeLocation, PlayerCenter, ECC_Visibility, Params);
      if (bHitEye && IsBlockedByEnvironment(SightHit))
      {
        CancelAimingAnim();
        return;
      }

      // 총구 기준 장애물 검사
      if (FVector::Distance(GetActorLocation(), CachedPlayer->GetActorLocation()) > 150.0f)
      {
        FHitResult MuzzleHit;
        bool bHitMuzzle = GetWorld()->LineTraceSingleByChannel(MuzzleHit, TraceStart, PlayerCenter, ECC_Visibility, Params);

        if (bHitMuzzle && IsBlockedByEnvironment(MuzzleHit))
        {
          CancelAimingAnim();
          return;
        }
      }
    }

    if (!bIsAimLocked)
    {
      // Z축(높이)을 무시한 평면(XY) 방향 계산
      FVector StartLoc = GetActorLocation();
      FVector TargetLoc = CachedPlayer->GetActorLocation();
      FVector DirXY = TargetLoc - StartLoc;
      DirXY.Z = 0.f;

      // 플레이어가 너무 가까이 있지 않을 때만 몸통 회전
      if (DirXY.SizeSquared() > 2500.f)
      {
        FRotator TargetYawRot = DirXY.GetSafeNormal().Rotation();
        FRotator NewRot = GetActorRotation();
        NewRot.Yaw = FMath::RInterpTo(GetActorRotation(), TargetYawRot, DeltaTime, 8.0f).Yaw;
        SetActorRotation(NewRot);
      }

      // 에임 상하(Pitch) 안정화
      FVector DirectionToSpine = (PlayerCenter - EyeLocation).GetSafeNormal();
      float TargetPitch = FRotator::NormalizeAxis(DirectionToSpine.Rotation().Pitch);

      // 코앞에 있을 때 허리가 뒤로 꺾이는 기괴한 현상을 방지
      TargetPitch = FMath::Clamp(TargetPitch, -60.0f, 60.0f);
      AimPitch = FMath::FInterpTo(AimPitch, TargetPitch, DeltaTime, 15.0f);

      // 나중에 쏠 방향과 레이저 끝점 갱신 (추적 중)
      LockedAimRotation = DirectionToSpine.Rotation();

      // 레이저 선은 총구(TraceStart)에서 출발해야 하므로 시작점을 맞춰줌.
      LockedTraceEnd = TraceStart + (DirectionToSpine * AttackRange);
    }

    // 점멸 타이밍(BlinkStartTime)이 되면 조준을 락(Lock)
    if (CurrentAimTime >= BlinkStartTime)
    {
      bIsAimLocked = true;
    }

    // 조준선 그리기 설정
#if !UE_BUILD_SHIPPING
    if (bShowDebugTrace)
    {
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
        DrawDebugLine(GetWorld(), TraceStart, LockedTraceEnd, LaserColor, false, 0.0f, 0, LineThickness);
      }
    }
#endif
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

  if (UCharacterMovementComponent* Movement = GetCharacterMovement())
  {
    Movement->MaxWalkSpeed = 0.f;
    Movement->bUseRVOAvoidance = false;
  }

  if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
  {
    if (AttackMontage)
    {
      AnimInstance->Montage_SetPlayRate(AttackMontage, 0.0f);
    }
  }

  float SafeAimTime = FMath::Max(0.1f, TotalAimTime);
  GetWorldTimerManager().SetTimer(AimTimerHandle, this, &ASanzoEnemy_Ranged::ResumeAiming, SafeAimTime, false);
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
  bIsAimLocked = false;
  CurrentAimTime = 0.f;
  GetWorldTimerManager().ClearTimer(AimTimerHandle);

  if (UCharacterMovementComponent* Movement = GetCharacterMovement())
  {
    Movement->MaxWalkSpeed = 350.f;
    Movement->bUseRVOAvoidance = true;
  }
}

void ASanzoEnemy_Ranged::CancelAimingAnim()
{
  StopAiming();

  if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
  {
    AnimInstance->Montage_Stop(0.2f, AttackMontage);
  }
}

void ASanzoEnemy_Ranged::OnStunEnteredCallback()
{
  CancelAimingAnim();
  Super::OnStunEnteredCallback();
}