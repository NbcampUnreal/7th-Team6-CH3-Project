#pragma once

#include "CoreMinimal.h"
#include "AI/SanzoEnemyBase.h"
#include "SanzoEnemy_Ranged.generated.h"

UCLASS()
class PROJECTSANZO_API ASanzoEnemy_Ranged : public ASanzoEnemyBase
{
  GENERATED_BODY()

public:
  ASanzoEnemy_Ranged();

  virtual void Attack() override;

  UFUNCTION(BlueprintCallable, Category = "Combat")
  void FireHitScan();

  virtual void Tick(float DeltaTime) override;

  UFUNCTION(BlueprintCallable, Category = "Combat|Aim")
  void StartAiming();

  UFUNCTION(BlueprintCallable, Category = "Combat|Aim")
  void StopAiming();

  // 조준 취소
  void CancelAimingAnim();

  // 조준 재개
  void ResumeAiming();
protected:
  // 원거리 적 전용 공격력
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
  float ShootDamage = 20.f;

  // Visual/Sound Effects
  UPROPERTY(EditAnywhere, Category = "Effects|Visual")
  TObjectPtr<class UParticleSystem> MuzzleFlashEffect;

  UPROPERTY(EditAnywhere, Category = "Effects|Visual")
  TObjectPtr<class UParticleSystem> TracerEffect;

  UPROPERTY(EditAnywhere, Category = "Effects|Visual")
  TObjectPtr<class UParticleSystem> HitEffect;

  UPROPERTY(EditAnywhere, Category = "Effects|Sound")
  TObjectPtr<class USoundBase> FireSound;

  // 디버그 라인 표시 여부
  UPROPERTY(EditAnywhere, Category = "Combat|Debug")
  bool bShowDebugTrace = true;

  // 조준 상태 플래그
  bool bIsAiming = false;

  // 조준을 시작한 후 흐른 시간
  float CurrentAimTime = 0.f;

  // 전체 조준 시간 (예: 0.5초 뒤 발사)
  UPROPERTY(EditAnywhere, Category = "Combat|Aim")
  float TotalAimTime = 1.5f;

  UPROPERTY(EditAnywhere, Category = "Combat|Aim")
  float BlinkStartTime = 1.0f;

  FTimerHandle AimTimerHandle;

  // 조준 고정 플래그
  bool bIsAimLocked = false;
  // 조준 고정된 위치
  FVector LockedTraceEnd;
  // 조준 고정된 각도
  FRotator LockedAimRotation;

  // 상하 에임 각도
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Aim")
  float AimPitch = 0.f;

  virtual void BeginPlay() override;

  // 플레이어 정보를 기억해둘 캐싱 변수
  UPROPERTY()
  ACharacter* CachedPlayer = nullptr;

  // 시야 검사를 위한 타이머
  float VisibilityCheckTimer = 0.f;

protected:
  virtual void OnStunEnteredCallback() override;
};
