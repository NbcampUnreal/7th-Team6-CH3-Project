#include "AI/SanzoEnemy_Boss.h"
#include "AI/Components/SanzoEnemyStunComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Common/SanzoLog.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"

ASanzoEnemy_Boss::ASanzoEnemy_Boss()
{
  PrimaryActorTick.bCanEverTick = true;

  // 보스 기본 스탯
  MaxHP = 1000.f;
  AttackRange = 300.f;
  MeleeDamage = 40.f;
  Exp = 500.f; // 레벨업 5번, 외모 업글 찍으라고 협박
}

void ASanzoEnemy_Boss::BeginPlay()
{
  Super::BeginPlay();
  bIsPhase2 = false;
  OriginalDamage = MeleeDamage;
}

float ASanzoEnemy_Boss::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
  // 스턴 상태일 때 받는 데미지 10% 증가
  float ModifiedDamage = DamageAmount;
  if (StunComponent && StunComponent->GetIsStunned())
  {
    ModifiedDamage *= 1.1f;
    UE_LOG(LogKDJ, Warning, TEXT("Boss is Stunned! Taking extra damage: %f"), ModifiedDamage);
  }

  float ActualDamage = Super::TakeDamage(ModifiedDamage, DamageEvent, EventInstigator, DamageCauser);

  // 체력이 50% 이하로 떨어졌고, 아직 2페이즈가 아니라면 페이즈 2 돌입
  if (!bIsPhase2 && CurrentHP <= (MaxHP * 0.5f) && !IsDead())
  {
    EnterPhase2();
  }

  return ActualDamage;
}

void ASanzoEnemy_Boss::EnterPhase2()
{
  bIsPhase2 = true;

  UE_LOG(LogKDJ, Error, TEXT("Boss Phase 2 Started!"));

  if (AAIController* AICon = Cast<AAIController>(GetController()))
  {
    if (UBlackboardComponent* BBComp = AICon->GetBlackboardComponent())
    {
      BBComp->SetValueAsBool(TEXT("IsPhase2"), true);
    }
  }

  // TO-DO: 포효 애니메이션 재생, 붉은 오라 이펙트 켜기 등 연출 추가
}

// 패턴 알림
void ASanzoEnemy_Boss::BroadcastAttackWarning(FName PatternName)
{
  if (GEngine)
  {
    FString DebugMsg = FString::Printf(TEXT("⚠️ [BOSS WARNING] Pattern Started: %s"), *PatternName.ToString());
    GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, DebugMsg);
  }

  UE_LOG(LogKDJ, Warning, TEXT("⚠️ [BOSS WARNING] Pattern Started: %s"), *PatternName.ToString());
  
  OnBossAttackWarning.Broadcast(PatternName);
}

// 돌진 패턴 실행 함수
void ASanzoEnemy_Boss::ExecuteDash()
{
  ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
  if (!Player || IsDead()) return;

  // 플레이어를 향한 방향 벡터 계산
  FVector StartLoc = GetActorLocation();
  FVector TargetLoc = Player->GetActorLocation();
  FVector Direction = (TargetLoc - StartLoc).GetSafeNormal2D();

  // 돌진하기 직전, 플레이어 쪽으로 몸을 회전
  SetActorRotation(Direction.Rotation());

  if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
  {
    MoveComp->SetMovementMode(MOVE_Falling);

    FVector FinalVelocity = Direction * DashSpeed;
    FinalVelocity.Z = 200.f;

    MoveComp->Velocity = FinalVelocity;
  }
}

void ASanzoEnemy_Boss::BeginHeavySmash()
{
  bIsHeavyAttack = true;

  // 내려찍기 시 기본 데미지의 2배
  MeleeDamage = OriginalDamage * 2.f;
}

void ASanzoEnemy_Boss::EndHeavySmash()
{
  bIsHeavyAttack = false;
  MeleeDamage = OriginalDamage;
}

void ASanzoEnemy_Boss::ExecuteSmashShockwave()
{
  // 충격파가 터질 중심점 계산
  FVector ImpactLocation;
  if (StaticWeaponMesh)
  {
    ImpactLocation = StaticWeaponMesh->GetSocketLocation(SocketEndName);
  }
  else
  {
    ImpactLocation = GetActorLocation() + (GetActorForwardVector() * 100.0f);
  }

  // 팀킬 방지
  TArray<AActor*> IgnoredActors;
  IgnoredActors.Add(this);

  // 반경 3m(300.f) 내의 모든 액터에게 광역 데미지
  UGameplayStatics::ApplyRadialDamage(
    GetWorld(),
    ShockwaveDamage,
    ImpactLocation,
    ShockwaveRadius,
    UDamageType::StaticClass(),
    IgnoredActors,
    this,
    GetController(),
    true
  );

  // [디버그용] 빨간색 원(공격 범위) 그리기
  if (GetWorld())
  {
    DrawDebugSphere(GetWorld(), ImpactLocation, ShockwaveRadius, 32, FColor::Red, false, 2.0f, 0, 2.0f);
  }
}