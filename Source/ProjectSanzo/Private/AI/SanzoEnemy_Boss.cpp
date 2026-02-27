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
#include "AI/SanzoEnemy_Boss_SwordAura.h"

ASanzoEnemy_Boss::ASanzoEnemy_Boss()
{
  PrimaryActorTick.bCanEverTick = true;

  // 보스 기본 스탯
  MaxHP = 1000.f;
  AttackRange = 500.f;
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

  AAIController* AICon = Cast<AAIController>(GetController());
  if (AICon && AICon->GetBlackboardComponent())
  {
    AICon->GetBlackboardComponent()->SetValueAsBool(TEXT("bIsPhase2"), true);
  }

  // (선택) 여기서 2페이즈 돌입 포효 몽타주 재생, 무기 파티클 변경 등 연출 추가
  if (Phase2RoarMontage)
  {
    PlayAnimMontage(Phase2RoarMontage);
  }
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

void ASanzoEnemy_Boss::FireSwordAura()
{
  // 검기 클래스가 등록되어 있지 않으면 취소
  if (!SwordAuraClass) return;

  ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
  if (!Player) return;

  // 발사 위치
  FVector SpawnLocation = StaticWeaponMesh ? StaticWeaponMesh->GetSocketLocation(SocketEndName) : GetActorLocation() + GetActorForwardVector() * 100.f;

  // 발사 방향
  FVector DirectionToPlayer = (Player->GetActorLocation() - SpawnLocation).GetSafeNormal();
  FRotator BaseRotation = DirectionToPlayer.Rotation();

  // 스폰 파라미터
  FActorSpawnParameters SpawnParams;
  SpawnParams.Owner = this;
  SpawnParams.Instigator = this;

  TArray<float> SpreadAngles = { -40.f, -20.f, 0.f, 20.f, 40.f };

  for (float Angle : SpreadAngles)
  {
    FRotator SpawnRotation = BaseRotation;
    SpawnRotation.Yaw += Angle;

    // 검기 생성
    GetWorld()->SpawnActor<ASanzoEnemy_Boss_SwordAura>(
      SwordAuraClass,
      SpawnLocation,
      SpawnRotation,
      SpawnParams
    );
  }
}

// 궁극기 시작 - 데미지 50% 증가, 분노 상태
void ASanzoEnemy_Boss::BeginUltimateFlurry()
{
  bIsUltimateFlurry = true;
  bIsEnraged = true;
  MeleeDamage = OriginalDamage * 1.5f;
}

// 궁극기 종료 - 데미지 원상 복구, 분노 상태 해제
void ASanzoEnemy_Boss::EndUltimateFlurry()
{
  bIsUltimateFlurry = false;
  bIsEnraged = false;
  MeleeDamage = OriginalDamage;
}

void ASanzoEnemy_Boss::OnParriedCallback()
{
  // 데미지를 즉시 기본값으로 복구
  bIsHeavyAttack = false;
  MeleeDamage = OriginalDamage;
  DisableWeaponCollision();
  Super::OnParriedCallback();
}
