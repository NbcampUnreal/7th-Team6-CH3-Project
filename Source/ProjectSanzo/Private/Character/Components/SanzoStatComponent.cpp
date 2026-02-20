// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/SanzoStatComponent.h"
#include "TimerManager.h"
#include "Common/SanzoLog.h"

USanzoStatComponent::USanzoStatComponent()
{
	AimingTag = FGameplayTag::RequestGameplayTag(FName("Character.Action.Aiming"));
	SprintTag = FGameplayTag::RequestGameplayTag(FName("Character.Action.Sprint"));
	AttackTag = FGameplayTag::RequestGameplayTag(FName("Character.Action.Attack"));
	ExhaustedTag = FGameplayTag::RequestGameplayTag(FName("Character.Status.Exhausted"));

	CurrentStamina = 100.f;
	MaxStamina = 100.f;
	StaminaRestoreAmount = 10.f; //초당 Stamina회복량
  SprintStaminaCost = 25.f; //Sprint 초당 소모량
  bIsExhausted = false;
	//테스트 코드
	CurrentHealth = 100.f;
	MaxHealth = 100.f;

	CurrentExp = 0.f;
	MaxExp = 100.f;
	Level = 1;



	


	PrimaryComponentTick.bCanEverTick = true;
	// ...
}


void USanzoStatComponent::BeginPlay()
{
	Super::BeginPlay();

	//스태미나 지속 회복
  GetWorld()->GetTimerManager().SetTimer(
		StaminaRestoreHandle, 
		[this]() //인자(회복량)를 받아야 해서 람다로 변환
		{
      RestoreStamina(StaminaRestoreAmount * 0.01f);
				BroadCastStatUpdate();
		},
		0.01f, 
		true);
	
#pragma  region InitalBroadCast
	BroadCastStatUpdate();
#pragma endregion	이준로
	// ...
}

void USanzoStatComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	//타이머 정리
	GetWorld()->GetTimerManager().ClearTimer(StaminaRestoreHandle);
  GetWorld()->GetTimerManager().ClearTimer(SprintStaminaCostHandle);
}


void USanzoStatComponent::RequestConsumeStaminaForSprint(bool bShouldConsume)
{
  bool bIsTimerActive = GetWorld()->GetTimerManager().IsTimerActive(SprintStaminaCostHandle);
	if(bShouldConsume == true && !bIsTimerActive) //true면서 타이머가없을때	소모
	{
		GetWorld()->GetTimerManager().SetTimer(
			SprintStaminaCostHandle, 
			[this]()
			{
				ConsumeStamina(SprintStaminaCost * 0.01f);
				BroadCastStatUpdate();
			},
			0.01f, 
      true);
	}
	else if(!bShouldConsume && bIsTimerActive)
	{
		GetWorld()->GetTimerManager().ClearTimer(SprintStaminaCostHandle);
	}
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
  if (CurrentStamina <= 0.f)
  {
    CurrentStamina = 0.f;
		bIsExhausted = true;
    return;
  }
  
	BroadCastStatUpdate();
}

void USanzoStatComponent::RestoreStamina(float Amount)
{
	//행동 중이면 회복 중지
	if (CheckTag(FGameplayTag::RequestGameplayTag(FName("Character.Action"))))
	{
		return;
	}
	if (CurrentStamina <= MaxStamina)
	{

		CurrentStamina = FMath::Clamp(CurrentStamina + Amount, 0.f, MaxStamina);
		BroadCastStatUpdate();
	}
}

bool USanzoStatComponent::bCanSprint()
{


	return false;
}

bool USanzoStatComponent::CheckTag(const FGameplayTag& Tag) const
{
	if(TagCheckDelegate.IsBound())
	{
		return TagCheckDelegate.Execute(Tag);
  }
  return false;
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

#pragma region PlayerApplyDamage
void USanzoStatComponent::ApplyDamage(float DamageAmount)
{
  CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.f, MaxHealth);
  BroadCastStatUpdate();

  UE_LOG(LogKDJ, Warning, TEXT("StatComponent: Health Reduced. Current Health: %f"), CurrentHealth);
}

bool USanzoStatComponent::IsDead() const
{
  return CurrentHealth <= 0.f;
}
#pragma endregion 김동주
