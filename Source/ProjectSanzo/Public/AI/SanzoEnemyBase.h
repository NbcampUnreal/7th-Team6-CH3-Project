// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SanzoEnemyBase.generated.h"

class UBehaviorTree;
class ASanzoRoomBase;

UCLASS()
class PROJECTSANZO_API ASanzoEnemyBase : public ACharacter
{
  GENERATED_BODY()

public:
  ASanzoEnemyBase();

protected:
  virtual void BeginPlay() override;

public:
  //virtual void Tick(float DeltaTime) override;

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
  bool IsDead() const;
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
  void Attack();

  UFUNCTION(BlueprintCallable, Category = "Combat")
  void Fire();

protected:
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
  TSubclassOf<AActor> ProjectileClass;

  UPROPERTY(EditAnywhere, Category = "Combat")
  UAnimMontage* AttackMontage;
#pragma endregion 김동주

#pragma region RoomBase Instance
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room")
  ASanzoRoomBase* CurrentRoom;
#pragma endregion 최윤서
};
