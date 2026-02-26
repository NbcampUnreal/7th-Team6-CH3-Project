#pragma once

#include "CoreMinimal.h"
#include "AI/SanzoEnemyBase.h"
#include "SanzoEnemy_MeleeBase.generated.h"

UCLASS()
class PROJECTSANZO_API ASanzoEnemy_MeleeBase : public ASanzoEnemyBase
{
	GENERATED_BODY()
	
public:
  ASanzoEnemy_MeleeBase();

  // 매 프레임 트레이스를 수행하기 위해 Tick 사용
  virtual void Tick(float DeltaTime) override;

  virtual bool CanAttack(AActor* Target) override;

  // 무기 콜리전 켜기
  UFUNCTION(BlueprintCallable, Category = "Combat|Melee")
  virtual void EnableWeaponCollision();

  // 무기 콜리전 끄기
  UFUNCTION(BlueprintCallable, Category = "Combat|Melee")
  virtual void DisableWeaponCollision();

protected:
  // 근접 공격 데미지
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Melee")
  float MeleeDamage = 30.0f;

  // 무기 트레이스 활성화 플래그
  bool bIsWeaponActive = false;

  // 무기 시작 부분 소켓 이름
  UPROPERTY(EditDefaultsOnly, Category = "Combat|Trace")
  FName SocketStartName = TEXT("TraceStart");

  // 무기 끝 부분 소켓 이름
  UPROPERTY(EditDefaultsOnly, Category = "Combat|Trace")
  FName SocketEndName = TEXT("TraceEnd");

  // 무기 트레이스 반경
  UPROPERTY(EditDefaultsOnly, Category = "Combat|Trace")
  float TraceRadius = 10.0f;

  // 중복 타격 방지 배열
  UPROPERTY()
  TArray<AActor*> HitActorsToIgnore;

  // 실제 트레이스를 수행할 함수
  virtual void PerformWeaponTrace();
};
