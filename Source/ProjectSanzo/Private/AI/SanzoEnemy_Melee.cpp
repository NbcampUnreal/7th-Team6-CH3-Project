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

  // 근접 공격용 박스 콜리전 생성
  MeleeCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("MeleeCollision"));

  // 무기에 콜리전을 부착
  MeleeCollision->SetupAttachment(StaticWeaponMesh);

  // 칼을 휘두를 때만 충돌 판정이 활성화되도록 초기에는 비활성화
  MeleeCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
  MeleeCollision->SetCollisionObjectType(ECC_WorldDynamic);
  MeleeCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
  MeleeCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

  // 오버랩 이벤트 연결
  MeleeCollision->OnComponentBeginOverlap.AddDynamic(this, &ASanzoEnemy_Melee::OnMeleeOverlap);
}

void ASanzoEnemy_Melee::EnableWeaponCollision()
{
  // 공격 판정 시작
  MeleeCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ASanzoEnemy_Melee::DisableWeaponCollision()
{
  // 공격 판정 종료
  MeleeCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASanzoEnemy_Melee::OnMeleeOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
  if (OtherActor && OtherActor != this)
  {
    UGameplayStatics::ApplyDamage(
      OtherActor,
      MeleeDamage,
      GetController(),
      this,
      UDamageType::StaticClass()
    );

    // 한 번 때리면 콜리전을 즉시 비활성화 
    DisableWeaponCollision();
  }
}

void ASanzoEnemy_Melee::Attack()
{
  if (GEngine)
  {
    GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("Melee Attack! Drawing Range..."));
  }
  Super::Attack();
  DrawAttackRange();
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

void ASanzoEnemy_Melee::DrawAttackRange()
{
  UWorld* World = GetWorld();
  if (!World) return;

  FVector Center = GetActorLocation();
  FVector Forward = GetActorForwardVector();

  float Radius = AttackRange;
  float AngleHalf = 45.0f; // 좌우 45도 (총 90도)

  int32 Segments = 50;

  // 색상 설정 (내부는 반투명, 외곽선은 진하게)
  FColor FillColor = FColor(255, 0, 0, 100);
  FColor OutlineColor = FColor::Red;

  // 왼쪽 가장자리 방향 벡터 계산
  FVector LeftEdgeDir = Forward.RotateAngleAxis(-AngleHalf, FVector::UpVector);
  float AngleStep = (AngleHalf * 2.f) / Segments;

  FVector PrevPoint = Center + (LeftEdgeDir * Radius);

  for (int32 i = 0; i <= Segments; i++)
  {
    FVector CurrentDir = LeftEdgeDir.RotateAngleAxis(AngleStep * i, FVector::UpVector);
    FVector CurrentPoint = Center + (CurrentDir * Radius);

    // 내부 채우기
    DrawDebugLine(World, Center, CurrentPoint, FillColor, false, 1.0f, 0, 8.0f);

    // 바깥쪽 호 외곽선
    if (i > 0)
    {
      DrawDebugLine(World, PrevPoint, CurrentPoint, OutlineColor, false, 1.0f, 0, 3.0f);
    }
    PrevPoint = CurrentPoint;
  }

  // 양옆 직선 외곽선
  FVector RightEdgeDir = Forward.RotateAngleAxis(AngleHalf, FVector::UpVector);
  DrawDebugLine(World, Center, Center + (LeftEdgeDir * Radius), OutlineColor, false, 1.0f, 0, 3.0f);
  DrawDebugLine(World, Center, Center + (RightEdgeDir * Radius), OutlineColor, false, 1.0f, 0, 3.0f);
}