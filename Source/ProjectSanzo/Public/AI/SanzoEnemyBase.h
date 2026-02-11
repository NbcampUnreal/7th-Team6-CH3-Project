// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SanzoEnemyBase.generated.h"

class UBehaviorTree;

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

public:
  UFUNCTION(BlueprintCallable, Category = "Stats")
  bool IsDead() const;
#pragma endregion

#pragma region Combat
protected:
  // 헤드샷 배율
  UPROPERTY(EditAnywhere, Category = "Combat")
  float HeadShotMultiplier = 2.5f;

  // 머리 뼈 이름
  UPROPERTY(EditAnywhere, Category = "Combat")
  FName HeadBoneName = TEXT("head");

public:
  // 데미지 처리 함수 오버라이드
  virtual float TakeDamage
  (
    float DamageAmount,
    struct FDamageEvent const& DamageEvent,
    class AController* EventInstigator,
    AActor* DamageCauser
  ) override;
#pragma endregion

};
