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
DECLARE_DELEGATE_RetVal_OneParam(bool, FOnTagCheckDelegate, const FGameplayTag&, State);
#pragma endregion 이준로

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTSANZO_API USanzoStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USanzoStatComponent();

protected:
	virtual void BeginPlay() override;

	//stamina 관련 변수
	float CurrentStamina;
	float MaxStamina;


	//Health 관련 변수
	float CurrentHealth;
	float MaxHealth;


	//Exp 관련 변수
	float CurrentExp;
	float MaxExp;
	int32 Level;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	void ConsumeStamina(float Amount);
	float GetStamina() const { return CurrentStamina; }
	bool bCanSprint();

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
