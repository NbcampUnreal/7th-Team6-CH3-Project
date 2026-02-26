#pragma once

#include "CoreMinimal.h"
#include "AI/SanzoEnemyBase.h"
#include "SanzoEnemy_Boss.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossAttackWarning, FName, PatternName);

UCLASS()
class PROJECTSANZO_API ASanzoEnemy_Boss : public ASanzoEnemyBase
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

  // 보스의 근접 공격 데미지
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Combat")
  float MeleeDamage = 40.f;

  // 트레이스 활성화 여부 플래그
  bool bIsWeaponActive = false;

  // 무기 시작 부분 소켓 이름
  UPROPERTY(EditDefaultsOnly, Category = "Boss|Combat|Trace")
  FName SocketStartName = TEXT("TraceStart");

  // 무기 끝 부분 소켓 이름
  UPROPERTY(EditDefaultsOnly, Category = "Boss|Combat|Trace")
  FName SocketEndName = TEXT("TraceEnd");

  // 무기 트레이스 반경
  UPROPERTY(EditDefaultsOnly, Category = "Boss|Combat|Trace")
  float TraceRadius = 15.0f;

  // 중복 타격 방지 배열
  UPROPERTY()
  TArray<AActor*> HitActorsToIgnore;

public:
  // 매 프레임 트레이스를 수행하기 위해 Tick 사용
  virtual void Tick(float DeltaTime) override;

  // 무기 콜리전 켜기
  UFUNCTION(BlueprintCallable, Category = "Boss|Combat")
  void EnableWeaponCollision();

  // 무기 콜리전 끄기
  UFUNCTION(BlueprintCallable, Category = "Boss|Combat")
  void DisableWeaponCollision();

private:
  // 실제 트레이스 로직을 수행할 함수
  void PerformWeaponTrace();
};
