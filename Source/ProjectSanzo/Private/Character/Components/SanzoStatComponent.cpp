// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/SanzoStatComponent.h"

USanzoStatComponent::USanzoStatComponent()
{
	CurrentStamina = 100.f;
	MaxStamina = 100.f;
	StaminaRegenRate = 5.f;
	StaminaConsumptionRate = 10.f;
	StaminaThresholdForActions = 20.f;
	StaminaRegenDelay = 2.f;
	TimeSinceLastStaminaUse = 0.f;
	DodgeStaminaCost = 15.f;
	SprintStaminaCostPerSecond = 5.f;

	//테스트 코드
	CurrentHealth = 100.f;
	MaxHealth = 100.f;

	CurrentExp = 0.f;
	MaxExp = 100.f;
	Level = 1;

	PrimaryComponentTick.bCanEverTick = false;
	// ...
}


void USanzoStatComponent::BeginPlay()
{
	Super::BeginPlay();

#pragma  region InitalBroadCast
	BroadCastStatUpdate();
#pragma endregion	이준로
	// ...
}


void USanzoStatComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USanzoStatComponent::ConsumeStamina(float Amount)
{
	CurrentStamina -= Amount;


	if (Amount <= 0.f)
	{
	}
}
#pragma region UIUpdateBroadCastStat
// 스탯 변경 방송 함수
// 스탯 변경되는 모든 함수 끝에 붙여주셔야 합니다!
void USanzoStatComponent::BroadCastStatUpdate()
{
	if (OnStatChanged.IsBound())
	{
		OnStatChanged.Broadcast(MakeStatData());
	}
}

// 변경 스탯 구조체 저장 함수
FSanzoStatData USanzoStatComponent::MakeStatData() const
{
	FSanzoStatData Data;
	Data.HealthPercent = (MaxHealth > 0.f) ? CurrentHealth / MaxHealth : 0.f;
	Data.StaminaPercent = (MaxStamina > 0.f) ? CurrentStamina / MaxStamina : 0.0f;
	Data.ExpPercent = (MaxExp > 0.f) ? CurrentExp / MaxExp : 0.f;
	Data.CurrentLevel = Level;

	return Data;
}
#pragma endregion 이준로
