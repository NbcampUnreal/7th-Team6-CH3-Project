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
  Exp = 500.f; // 레벨업 5번, 외모 업글 찍으라고 협박
}

void ASanzoEnemy_Boss::BeginPlay()
{
  Super::BeginPlay();
  bIsPhase2 = false;
}

void ASanzoEnemy_Boss::Tick(float DeltaTime)
{
  if (bIsWeaponActive)
  {
    PerformWeaponTrace();
  }
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

// 무기 콜리전 활성화
void ASanzoEnemy_Boss::EnableWeaponCollision()
{
  // 공격 시작
  bIsWeaponActive = true;
  // 맞은 적 목록 초기화
  HitActorsToIgnore.Empty();
  HitActorsToIgnore.Add(this);

  // 트레이스 시작 전, 현재 소켓 위치를 '이전 위치'로 초기화
  if (StaticWeaponMesh)
  {
    PrevStartLocation = StaticWeaponMesh->GetSocketLocation(SocketStartName);
    PrevEndLocation = StaticWeaponMesh->GetSocketLocation(SocketEndName);
  }
}

// 무기 콜리전 비활성화
void ASanzoEnemy_Boss::DisableWeaponCollision()
{
  // 공격 종료
  bIsWeaponActive = false;
}

void ASanzoEnemy_Boss::PerformWeaponTrace()
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