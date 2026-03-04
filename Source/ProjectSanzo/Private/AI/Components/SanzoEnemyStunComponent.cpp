#include "AI/Components/SanzoEnemyStunComponent.h"

USanzoEnemyStunComponent::USanzoEnemyStunComponent()
{
  PrimaryComponentTick.bCanEverTick = false;
}

void USanzoEnemyStunComponent::NotifyParried(int32 StunAmount)
{
  OnParried.Broadcast();
  AddStunGauge(StunAmount);
}

void USanzoEnemyStunComponent::AddStunGauge(int32 Amount)
{
  // 이미 스턴 상태인 경우 무시
  if (bIsStunned) return;

  // 스턴 게이지 추가 및 최대값 클램프
  CurrentStunCount = FMath::Clamp(CurrentStunCount + Amount, 0, MaxStunCount);

  // 스턴 게이지 변경 이벤트 브로드캐스트
  OnStunCountChanged.Broadcast(CurrentStunCount, MaxStunCount);

  // 스턴 게이지가 최대값에 도달하면 스턴 상태 진입
  if (CurrentStunCount >= MaxStunCount)
  {
    EnterStunState();
  }
}

void USanzoEnemyStunComponent::EnterStunState()
{
  // 스턴 상태 진입
  bIsStunned = true;

  // 스턴 상태 진입 이벤트 브로드캐스트
  OnStunStateEntered.Broadcast();

  // 타이머 설정하여 스턴 해제 예약
  if (UWorld* World = GetWorld())
  {
    GetWorld()->GetTimerManager().SetTimer(StunTimerHandle, this, &USanzoEnemyStunComponent::RecoverFromStun, StunDuration, false);
  }
}

void USanzoEnemyStunComponent::RecoverFromStun()
{
  // 스턴 상태 해제
  bIsStunned = false;

  // 스턴 게이지 초기화
  CurrentStunCount = 0;

  // 스턴 게이지 변경 이벤트 브로드캐스트
  OnStunCountChanged.Broadcast(CurrentStunCount, MaxStunCount);

  // 스턴 상태 해제 이벤트 브로드캐스트
  OnStunStateRecovered.Broadcast();
}
