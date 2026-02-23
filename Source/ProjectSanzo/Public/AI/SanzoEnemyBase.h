#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AI/Interface/SanzoEnemyInterface.h"
#include "Components/SphereComponent.h"
#include "SanzoEnemyBase.generated.h"

class UWidgetComponent;
class UBehaviorTree;
class ASanzoRoomBase;

UCLASS()
class PROJECTSANZO_API ASanzoEnemyBase : public ACharacter, public ISanzoEnemyInterface
{
  GENERATED_BODY()

public:
  ASanzoEnemyBase();

protected:
  virtual void BeginPlay() override;

public:
  virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
  TObjectPtr<UBehaviorTree> EnemyBehaviorTree;

#pragma region EnemyStats
protected:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
  float MaxHP = 100.f;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
  float CurrentHP;

  // 기본 근거리 사거리
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
  float AttackRange = 150.f;

  // 중복 사망 처리를 막기 위한 플래그
  bool bIsDead = false;

public:
  UFUNCTION(BlueprintCallable, Category = "Stats")
  virtual bool IsDead() const override;

  virtual float GetAttackRange() const override;

public:
  // 상하 조준 각도 (-90 ~ 90)
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
  float AimPitch = 0.f;
#pragma endregion 김동주

#pragma region EnemyTakeDamage
public:
  // 데미지 처리 함수 오버라이드
  virtual float TakeDamage
  (
    float DamageAmount,
    struct FDamageEvent const& DamageEvent,
    class AController* EventInstigator,
    AActor* DamageCauser
  ) override;

protected:
  virtual void Die();
#pragma endregion 김동주

#pragma region EnemyAttack
public:
  virtual void Attack() override;

  // 공격 조건을 검사하는 가상 함수
  virtual bool CanAttack(AActor* Target);
protected:
  // 무기 컴포넌트(스켈레탈 매시) 
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
  TObjectPtr<class USkeletalMeshComponent> WeaponMesh;

  // 무기 컴포넌트(스태틱 매시)
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
  TObjectPtr<class UStaticMeshComponent> StaticWeaponMesh;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
  TObjectPtr<class UAnimMontage> AttackMontage;
#pragma endregion 김동주

#pragma region RoomBase Instance
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room")
  ASanzoRoomBase* CurrentRoom;
#pragma endregion 최윤서

#pragma region OverHeadUI

public:
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
  UWidgetComponent* OverHeadHPBar;

  FTimerHandle OverHeadHPBarUpdateTimerHandle;

  void UpdateOverHeadHPBar();

  void MakeOverHeadHPBar3D();

#pragma endregion 이준로

#pragma region AlertUI
public:
  // 적이 플레이어를 감지했을 때 나타나는 UI 위젯 컴포넌트
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI|Alert")
  TObjectPtr<UWidgetComponent> AlertWidgetComp;

  // 플레이어 감지 시 AlertWidget을 활성화하는 함수
  void ShowAlertWidget(bool bIsSight);

  // 플레이어 감지 해제 시 AlertWidget을 비활성화하는 함수
  void HideAlertWidget();

  // AlertWidget의 UI를 업데이트하는 함수
  UFUNCTION(BlueprintImplementableEvent, Category = "UI|Alert")
  void OnUpdateAlertUI(bool bIsSight);
protected:
  // AlertWidget 타이머 핸들
  FTimerHandle AlertWidgetTimerHandle;
#pragma endregion 김동주

#pragma region ProximitySensor
public:
  // 적이 플레이어와 가까워졌는지 감지하는 구체형 센서 컴포넌트
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sensor")
  TObjectPtr<USphereComponent> ProximitySensor;

  // 구체 센서의 겹침 이벤트를 처리하는 함수
  UFUNCTION()
  void OnProximityOverlap(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult);
#pragma endregion 김동주
};
