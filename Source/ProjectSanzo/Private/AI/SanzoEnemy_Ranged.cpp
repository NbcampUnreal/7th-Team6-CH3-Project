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
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

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

  if (UCharacterMovementComponent* Movement = GetCharacterMovement())
  {
    DefaultWalkSpeed = Movement->MaxWalkSpeed;
  }
}

void ASanzoEnemy_Ranged::Attack()
{
  Super::Attack();
}

void ASanzoEnemy_Ranged::FireHitScan()
{
  if (IsDead() || !StaticWeaponMesh) return;

  FVector TraceStart = GetActorLocation();

  if (StaticWeaponMesh->DoesSocketExist(TEXT("MuzzleFlash")))
  {
    TraceStart = StaticWeaponMesh->GetSocketLocation(TEXT("MuzzleFlash"));
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
    // 사운드 큐로 변경 - 최윤서
    UGameplayStatics::PlaySoundAtLocation(
      this,
      FireSound,
      TraceStart,
      1.f,
      1.f,
      0.f,
      EnemyAttenuation);
  }

  // 히트스캔 (LineTrace)
  FCollisionQueryParams QueryParams;
  QueryParams.AddIgnoredActor(this);

  FHitResult ValidHitResult;
  bool bValidHit = false;
  FVector TraceEndPoint = TraceEnd;

  // 최대 10번까지 관통 시도 (무한 루프 방지)
  for (int32 i = 0; i < 10; ++i)
  {
    FHitResult HitResult;
    bool bHit = GetWorld()->LineTraceSingleByChannel(
      HitResult,
      TraceStart,
      TraceEnd,
      ECC_Visibility,
      QueryParams
    );

    if (bHit)
    {
      AActor* HitActor = HitResult.GetActor();

      if (HitActor && HitActor->IsA<ASanzoEnemyBase>())
      {
        QueryParams.AddIgnoredActor(HitActor);
        continue;
      }

      ValidHitResult = HitResult;
      TraceEndPoint = HitResult.ImpactPoint;
      bValidHit = true;
      break;
    }
    else
    {
      break;
    }
  }

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
  if (bValidHit)
  {
    AActor* HitActor = ValidHitResult.GetActor();
    if (HitActor)
    {
      UGameplayStatics::ApplyDamage(
        HitActor,
        ShootDamage,
        GetController(),
        this,
        UDamageType::StaticClass()
      );

#pragma region NotifyParried
      if (ACharacter* Character = Cast<ACharacter>(ValidHitResult.GetActor()))
      {
        if (IGameplayTagAssetInterface* TagCheck = Cast<IGameplayTagAssetInterface>(Character))
        {
          if (TagCheck->HasMatchingGameplayTag(SanzoTags::ParryWindow))
          {
            StunComponent->NotifyParried();
          }

          //회피성공
          if (TagCheck->HasMatchingGameplayTag(SanzoTags::IFrame))
          {
            return;
          }
        }
      }

#pragma endregion 김형백

      // 피격 위치에 이펙트 생성
      if (HitEffect)
      {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEffect, ValidHitResult.ImpactPoint, ValidHitResult.ImpactNormal.Rotation());
      }



    }

    // [디버그용] 명중 시 초록색 선 출력
    if (bShowDebugTrace)
    {
      DrawDebugLine(GetWorld(), TraceStart, ValidHitResult.ImpactPoint, FColor::Green, false, 2.0f, 0, 1.0f);
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

  // 죽었거나, 무기가 없거나, 타겟이 없으면 즉시 종료
  if (IsDead() || !StaticWeaponMesh || !CachedPlayer) return;

  VisibilityCheckTimer += DeltaTime;
  bool bNeedsVisibilityCheck = (VisibilityCheckTimer >= 0.1f);

  if (bIsAiming || bNeedsVisibilityCheck)
  {
    static const FName MuzzleSocketName = TEXT("MuzzleFlash");
    static const FName SpineSocketName = TEXT("spine_04");

    // 총구 및 플레이어 위치 계산
    FVector TraceStart = StaticWeaponMesh->DoesSocketExist(MuzzleSocketName) ?
      StaticWeaponMesh->GetSocketLocation(MuzzleSocketName) : GetActorLocation();

    FVector PlayerCenter = CachedPlayer->GetMesh()->DoesSocketExist(SpineSocketName) ?
      CachedPlayer->GetMesh()->GetSocketLocation(SpineSocketName) : CachedPlayer->GetActorLocation();

    // 시야 차단 검사
    if (bNeedsVisibilityCheck)
    {
      VisibilityCheckTimer = 0.0f;
      FVector EyeLocation = GetMesh()->GetSocketLocation(TEXT("head"));

      auto IsBlockedByEnvironment = [&](const FHitResult& Hit) -> bool {
        AActor* HitActor = Hit.GetActor();
        return HitActor && !HitActor->ActorHasTag("Player") && !HitActor->IsA<ASanzoEnemyBase>();
        };

      FCollisionQueryParams Params;
      Params.AddIgnoredActor(this);
      bool bIsBlocked = false;
      FHitResult LineTraceHit;

      // 눈 기준 검사
      if (GetWorld()->LineTraceSingleByChannel(LineTraceHit, EyeLocation, PlayerCenter, ECC_Visibility, Params) && IsBlockedByEnvironment(LineTraceHit))
      {
        bIsBlocked = true;
      }
      // 총구 기준 검사
      else if (FVector::Distance(GetActorLocation(), CachedPlayer->GetActorLocation()) > 150.0f)
      {
        if (GetWorld()->LineTraceSingleByChannel(LineTraceHit, TraceStart, PlayerCenter, ECC_Visibility, Params) && IsBlockedByEnvironment(LineTraceHit))
        {
          bIsBlocked = true;
        }
      }

      if (AAIController* AICon = Cast<AAIController>(GetController()))
      {
        if (UBlackboardComponent* BBComp = AICon->GetBlackboardComponent())
        {
          BBComp->SetValueAsBool(TEXT("bIsAimBlocked"), bIsBlocked);
        }
      }

      // 조준 중인데 시야가 막혔다면 즉시 취소
      if (bIsAiming && bIsBlocked)
      {
        CancelAimingAnim();
      }
    }

    if (bIsAiming)
    {
      UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

      // 사거리 이탈 및 몽타주 종료 검사
      if (FVector::Distance(GetActorLocation(), CachedPlayer->GetActorLocation()) > AttackRange ||
        (AnimInstance && !AnimInstance->Montage_IsPlaying(AttackMontage)))
      {
        CancelAimingAnim();
        return;
      }

      CurrentAimTime += DeltaTime;

      if (!bIsAimLocked)
      {
        FVector DirXY = PlayerCenter - TraceStart;
        DirXY.Z = 0.f;

        float DistSq = DirXY.SizeSquared();
        float RotationAlpha = FMath::Clamp((DistSq - 900.f) / (22500.f - 900.f), 0.0f, 1.0f);
        float FinalRotationSpeed = 15.0f * RotationAlpha;

        if (FinalRotationSpeed > 0.001f && !DirXY.IsNearlyZero())
        {
          FQuat TargetQuat = DirXY.GetSafeNormal().ToOrientationQuat();
          FQuat CurrentQuat = GetActorQuat();
          FQuat NewQuat = FMath::QInterpTo(CurrentQuat, TargetQuat, DeltaTime, FinalRotationSpeed);
          SetActorRotation(NewQuat);
        }

        // 상하(Pitch) 계산 및 레이저 갱신
        FVector DirectionToSpine = (FVector::Distance(PlayerCenter, TraceStart) < 100.f) ? GetActorForwardVector() : (PlayerCenter - TraceStart).GetSafeNormal();
        float TargetPitch = FMath::Clamp(FRotator::NormalizeAxis(DirectionToSpine.Rotation().Pitch), -60.0f, 60.0f);
        AimPitch = FMath::FInterpTo(AimPitch, TargetPitch, DeltaTime, 15.0f);

        LockedAimRotation = DirectionToSpine.Rotation();
        LockedTraceEnd = TraceStart + (DirectionToSpine * AttackRange);
      }

      if (CurrentAimTime >= BlinkStartTime) bIsAimLocked = true;

     // 디버그 라인
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
  }

  // 조준 중이 아닐 때 에임 원복
  if (!bIsAiming)
  {
    AimPitch = FMath::FInterpTo(AimPitch, 0.f, DeltaTime, 10.0f);
  }
}

void ASanzoEnemy_Ranged::StartAiming()
{
  if (bIsAiming) return;

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
    Movement->MaxWalkSpeed = DefaultWalkSpeed;
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