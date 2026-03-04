#include "AI/SanzoEnemy_MeleeBase.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "Common/SanzoGameplayTag.h"
#include "GameplayTagAssetInterface.h"
#include "AI/Components/SanzoEnemyStunComponent.h" 

ASanzoEnemy_MeleeBase::ASanzoEnemy_MeleeBase()
{
  PrimaryActorTick.bCanEverTick = true;
}

void ASanzoEnemy_MeleeBase::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

  if (bIsWeaponActive)
  {
    PerformWeaponTrace();
  }
}

bool ASanzoEnemy_MeleeBase::CanAttack(AActor* TargetActor)
{
  if (!TargetActor) return false;

  // 2D 평면상(Z축 무시)의 방향 벡터 추출
  FVector DirToTarget = (TargetActor->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
  FVector EnemyForward = GetActorForwardVector().GetSafeNormal2D();

  // 내적을 사용해 두 벡터 사이의 각도 계산
  float DotProduct = FMath::Clamp(FVector::DotProduct(EnemyForward, DirToTarget), -1.0f, 1.0f);
  float AngleToTarget = FMath::RadiansToDegrees(FMath::Acos(DotProduct));

  // 거리가 AttackRange 이하이고, 각도가 정면(좌우 45도, 총 90도) 이내일 때만 true
  float Distance = FVector::Distance(GetActorLocation(), TargetActor->GetActorLocation());

  if (Distance <= AttackRange && AngleToTarget <= 45.0f)
  {
    return true;
  }

  return false;
}

void ASanzoEnemy_MeleeBase::EnableWeaponCollision()
{
  bIsWeaponActive = true;
  HitActorsToIgnore.Empty();
  HitActorsToIgnore.Add(this);
}

void ASanzoEnemy_MeleeBase::DisableWeaponCollision()
{
  bIsWeaponActive = false;
}

void ASanzoEnemy_MeleeBase::PerformWeaponTrace()
{
  if (!StaticWeaponMesh) return;

  FVector SocketStart = StaticWeaponMesh->GetSocketLocation(SocketStartName);
  FVector SocketEnd = StaticWeaponMesh->GetSocketLocation(SocketEndName);

  FVector CurrentStart = SocketStart + FVector(0.f, 0.f, TraceZOffset);
  FVector OutputEnd = SocketEnd + FVector(0.f, 0.f, TraceZOffset);

  TArray<FHitResult> OutHits;
  TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
  ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

  bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
    GetWorld(),
    CurrentStart, OutputEnd,
    TraceRadius,
    ObjectTypes,
    false,
    HitActorsToIgnore,
    EDrawDebugTrace::None, // 테스트 후 None으로 변경
    OutHits,
    true,
    FLinearColor::Red, FLinearColor::Green, 2.0f
  );

  if (bHit)
  {
    for (const FHitResult& Hit : OutHits)
    {
      AActor* HitActor = Hit.GetActor();
      if (!HitActor || HitActorsToIgnore.Contains(HitActor)) continue;

      // 팀킬 방지
      if (HitActor->IsA<ASanzoEnemyBase>()) continue;

      if (HitActor && HitActor != this)
      {
        UGameplayStatics::ApplyDamage(
          HitActor,
          MeleeDamage,
          GetController(),
          this,
          UDamageType::StaticClass()
        );

#pragma region NotifyParried
        if (ACharacter* Character = Cast<ACharacter>(HitActor))
        {
          if (IGameplayTagAssetInterface* TagCheck = Cast<IGameplayTagAssetInterface>(Character))
          {
            if (TagCheck->HasMatchingGameplayTag(SanzoTags::ParryWindow))
            {
              // 작성자: 김동주
              // 특정 패턴 패리 시 스턴 게이지 2칸
              // 그 외 패리 시 1칸 증가하도록 구현
              int32 StunAmount = GetStunGaugeOnParried();
              StunComponent->NotifyParried(StunAmount);
            }

            //회피성공
            if (TagCheck->HasMatchingGameplayTag(SanzoTags::IFrame))
            {
              DisableWeaponCollision();
              return;
            }
          }
        }

#pragma endregion 김형백

        HitActorsToIgnore.Add(HitActor);
        DisableWeaponCollision();
      }
    }
  }
}