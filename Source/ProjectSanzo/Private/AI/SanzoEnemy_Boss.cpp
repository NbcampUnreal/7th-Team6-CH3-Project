#include "AI/SanzoEnemy_Boss.h"
#include "AI/Components/SanzoEnemyStunComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Common/SanzoLog.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"

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