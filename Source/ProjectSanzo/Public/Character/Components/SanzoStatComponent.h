// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "SanzoStatComponent.generated.h"


#pragma region UIStatData

USTRUCT(BlueprintType)
struct FSanzoStatData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	float HealthPercent = 0.0f;
	UPROPERTY(BlueprintReadOnly)
	float StaminaPercent = 0.0f;
	UPROPERTY(BlueprintReadOnly)
	float ExpPercent = 0.0f;
	UPROPERTY(BlueprintReadOnly)
	int32 CurrentLevel = 1;
};

//정보 전달 할 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatChangedDelegate, const FSanzoStatData&, StatData);

#pragma endregion 이준로
//추후 태그 추가/제거 델리게이트도 만들 예정 !
DECLARE_DELEGATE_RetVal_OneParam(bool, FOnTagCheckDelegate, const FGameplayTag&);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTSANZO_API USanzoStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USanzoStatComponent();

protected:
	virtual void BeginPlay() override;
  virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	//stamina 관련 변수
	UPROPERTY(EditAnywhere, Category = "Stamina")
	float CurrentStamina;
	UPROPERTY(EditAnywhere, Category = "Stamina")
	float MaxStamina;
  UPROPERTY(EditAnywhere, Category = "Stamina")
  float StaminaRestoreAmount;
  UPROPERTY(EditAnywhere, Category = "Stamina|Cost")
  float SprintStaminaCost;
	UPROPERTY(EditAnywhere, Category = "Stamina|Cost")
  float ParryStaminaCost;
	//스태미나 지속회복 핸들
	FTimerHandle StaminaRestoreHandle;
  FTimerHandle SprintStaminaCostHandle;
	//Health 관련 변수
	float CurrentHealth;
	float MaxHealth;

	//Exp 관련 변수
	float CurrentExp;
	float MaxExp;
	int32 Level;

	
	FGameplayTag AimingTag;
	FGameplayTag SprintTag;
	FGameplayTag AttackTag;
	FGameplayTag ExhaustedTag;
	void ConsumeStamina(float Amount);
	void RestoreStamina(float Amount);

	FTimerHandle ExhaustionRecoveryTimerHandle;
	void ExhaustionRecovery(); // 콜백함수
	

public:
	void BeginExhaustionCooldown(); //타이머함수
	FOnTagCheckDelegate TagCheckDelegate;
	//캐릭터에서 사용할 질문함수
	void RequestConsumeStaminaForSprint(bool bShouldConsume);
	void ConsumeStaminaForAction();
	bool bIsExhausted;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;




	float GetStamina() const { return CurrentStamina; }

	bool bCanSprint();
	//태그확인용 델리게이
  bool CheckTag(const FGameplayTag& Tag) const;

	
#pragma region UIDataTransfer

	FOnStatChangedDelegate OnStatChanged;

	void BroadCastStatUpdate();

	FSanzoStatData MakeStatData() const;

#pragma endregion 이준로

#pragma region PlayerApplyDamage
  void ApplyDamage(float DamageAmount);
  bool IsDead() const;
#pragma endregion 김동주
};
