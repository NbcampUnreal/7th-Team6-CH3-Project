#include "AI/SanzoEnemy_Melee.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Common/SanzoGameplayTag.h"
#include "AI/Components/SanzoEnemyStunComponent.h" 

ASanzoEnemy_Melee::ASanzoEnemy_Melee()
{
  // 근접 적 기본 스탯 세팅
  MaxHP = 120.f;
  AttackRange = 180.f;
  Exp = 30.f;
}

void ASanzoEnemy_Melee::Tick(float DeltaTime)
{
  if (bIsWeaponActive)
  {
    PerformWeaponTrace();
  }
}

// 무기 콜리전 활성화
void ASanzoEnemy_Melee::EnableWeaponCollision()
{
  // 공격 시작
  bIsWeaponActive = true;
  // 맞은 적 목록 초기화
  HitActorsToIgnore.Empty();
  HitActorsToIgnore.Add(this);
}

// 무기 콜리전 비활성화
void ASanzoEnemy_Melee::DisableWeaponCollision()
{
  // 공격 종료
  bIsWeaponActive = false;
}

void ASanzoEnemy_Melee::PerformWeaponTrace()
{
  if (!StaticWeaponMesh) return;

  // 현재 프레임의 소켓 위치 가져오기
  FVector CurrentStart = StaticWeaponMesh->GetSocketLocation(SocketStartName);
  FVector CurrentEnd = StaticWeaponMesh->GetSocketLocation(SocketEndName);

  // 트레이스 설정
  TArray<FHitResult> OutHits;
  TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
  ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

  // 스피어 스윕 수행
  // bTraceComplex=false, ActorsToIgnore=HitActorsToIgnore, DrawDebugType=EDrawDebugTrace::ForDuration (디버그용 표시)
  bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
    GetWorld(),
    CurrentStart, CurrentEnd, // 시작점과 끝점 변경!
    TraceRadius,
    ObjectTypes,
    false,
    HitActorsToIgnore,
    EDrawDebugTrace::ForDuration, // 테스트 후 None으로 끄기
    OutHits,
    true, // IgnoreSelf
    FLinearColor::Red, FLinearColor::Green, 2.0f
  );

  // 충돌 결과 처리
  if (bHit)
  {
    for (const FHitResult& Hit : OutHits)
    {
      AActor* HitActor = Hit.GetActor();
      if (!HitActor || HitActorsToIgnore.Contains(HitActor)) continue;

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
        // 한 번 맞은 적은 이번 공격에서 다시 맞지 않도록 목록에 추가
        HitActorsToIgnore.Add(HitActor);
        // 한 번 때리면 콜리전을 즉시 비활성화 
        DisableWeaponCollision();
      }
    }
  }
}

void ASanzoEnemy_Melee::Attack()
{
  if (GEngine)
  {
    GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("Melee Attack! Drawing Range..."));
  }
  Super::Attack();
}

bool ASanzoEnemy_Melee::CanAttack(AActor* TargetActor)
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