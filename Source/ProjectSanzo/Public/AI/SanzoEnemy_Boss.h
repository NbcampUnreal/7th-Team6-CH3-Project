#pragma once

#include "CoreMinimal.h"
#include "AI/SanzoEnemy_MeleeBase.h"
#include "SanzoEnemy_Boss.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossAttackWarning, FName, PatternName);

UCLASS()
class PROJECTSANZO_API ASanzoEnemy_Boss : public ASanzoEnemy_MeleeBase
{
  GENERATED_BODY()

public:
  ASanzoEnemy_Boss();

  // 데미지 처리 (10% 추가 데미지 및 페이즈 전환 체크)
  virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

  // 패턴 경고 알림
  UPROPERTY(BlueprintAssignable, Category = "Boss|Events")
  FOnBossAttackWarning OnBossAttackWarning;

  // AnimNotify에서 호출할 패턴 시작 함수
  UFUNCTION(BlueprintCallable, Category = "Boss|Combat")
  void BroadcastAttackWarning(FName PatternName);

protected:
  virtual void BeginPlay() override;

  // 현재 2페이즈인지 확인하는 플래그
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Phase")
  bool bIsPhase2 = false;

  // 페이즈 2 진입 처리
  void EnterPhase2();

public:
  // 돌진 실행 함수
  UFUNCTION(BlueprintCallable, Category = "Boss|Combat")
  void ExecuteDash();

  UPROPERTY(BlueprintReadOnly, Category = "Boss|Combat")
  bool bIsHeavyAttack = false;

  // 강공격 준비
  UFUNCTION(BlueprintCallable, Category = "Boss|Combat")
  void BeginHeavySmash();

  // 강공격 종료
  UFUNCTION(BlueprintCallable, Category = "Boss|Combat")
  void EndHeavySmash();

  // 강공격 충격파 실행
  UFUNCTION(BlueprintCallable, Category = "Boss|Combat")
  void ExecuteSmashShockwave();

  // 검기 방출
  UFUNCTION(BlueprintCallable, Category = "Boss|Combat")
  void FireSwordAura();

protected:
  // 돌진 속도
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Combat")
  float DashSpeed = 4000.f;

  // 원래 데미지를 기억해 둘 변수
  float OriginalDamage;

  // 충격파 반경 (3m = 300.0f)
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Combat")
  float ShockwaveRadius = 300.0f;

  // 충격파 데미지
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Combat")
  float ShockwaveDamage = 20.0f;

  // 검기 투사체 클래스
  UPROPERTY(EditDefaultsOnly, Category = "Boss|Combat")
  TSubclassOf<class ASanzoEnemy_Boss_SwordAura> SwordAuraClass;

#pragma region Sound
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects|Sound")
  TArray<USoundBase*> AttackSounds;
#pragma endregion 최윤서

  // 궁극기 패턴 플래그
  UPROPERTY(BlueprintReadOnly, Category = "Boss|Combat")
  bool bIsUltimateFlurry = false;

  // 궁극기 시작 (데미지 세팅)
  UFUNCTION(BlueprintCallable, Category = "Boss|Combat")
  void BeginUltimateFlurry();

  // 궁극기 종료 (원상 복구)
  UFUNCTION(BlueprintCallable, Category = "Boss|Combat")
  void EndUltimateFlurry();
};
