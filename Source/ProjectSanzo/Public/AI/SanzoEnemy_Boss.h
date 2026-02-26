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
};
