#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SanzoEnemyStunComponent.generated.h"

// 스턴 게이지 변경 시 UI 갱신
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStunCountChanged, int32, CurrentStunCount, int32, MaxStunCount);
// 스턴 상태 진입 시 애니메이션 및 AI 정지
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStunStateEntered);
// 스턴 상태 해제 시 AI 재개
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStunStateRecovered);
// 공격이 패리된 경우 스턴 게이지 증가 및 몽타주 재생
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnParried);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTSANZO_API USanzoEnemyStunComponent : public UActorComponent
{
	GENERATED_BODY()

public:
  USanzoEnemyStunComponent();

protected:
  // 최대 스턴 게이지 수
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stun")
  int32 MaxStunCount = 3;

  // 현재 스턴 게이지 수
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stun")
  int32 CurrentStunCount = 0;

  // 스턴 지속 시간
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stun")
  float StunDuration = 2.f;

  // 현재 스턴 상태 여부
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stun")
  bool bIsStunned = false;

  // 스턴 타이머 핸들
  FTimerHandle StunTimerHandle;

public:
  // 스턴 게이지 변경 시 UI 갱신
  UPROPERTY(BlueprintAssignable, Category = "Stun|Events")
  FOnStunCountChanged OnStunCountChanged;

  // 스턴 상태 진입 시 애니메이션 및 AI 정지
  UPROPERTY(BlueprintAssignable, Category = "Stun|Events")
  FOnStunStateEntered OnStunStateEntered;

  // 스턴 상태 해제 시 AI 재개
  UPROPERTY(BlueprintAssignable, Category = "Stun|Events")
  FOnStunStateRecovered OnStunStateRecovered;

  // 공격이 패리된 경우 스턴 게이지 증가 및 몽타주 재생
  UPROPERTY(BlueprintAssignable, Category = "Stun|Events")
  FOnParried OnParried;

  // '패링 당함' 알림 함수
  UFUNCTION(BlueprintCallable, Category = "Stun")
  void NotifyParried(int32 StunAmount = 1);

  // 스턴 게이지 추가 함수
  UFUNCTION(BlueprintCallable, Category = "Stun")
  void AddStunGauge(int32 Amount = 1);

  //최대 스턴카운트 가져오기 - 작업자: 이준로
  int32 GetMaxStunCount() const { return MaxStunCount; }
  int32 GetCurrentStunCount() const { return CurrentStunCount; }
  bool GetIsStunned() const { return bIsStunned; }

protected:
  // 스턴 상태 진입 처리 함수
  void EnterStunState();
  // 스턴 상태 해제 처리 함수
  void RecoverFromStun();
};
