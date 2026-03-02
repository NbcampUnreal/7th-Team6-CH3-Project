// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/SanzoStatComponent.h"
#include "Character/Interface/SanzoTagEditorInterface.h"
#include "TimerManager.h"
#include "Common/SanzoGameplayTag.h"
#include "Common/SanzoLog.h"
#include "Character/SanzoPlayerController.h"

USanzoStatComponent::USanzoStatComponent()
{
	AimingTag = FGameplayTag::RequestGameplayTag(FName("Character.Action.Movable.Aiming"));
	SprintTag = FGameplayTag::RequestGameplayTag(FName("Character.Action.Movable.Sprint"));
	AttackTag = FGameplayTag::RequestGameplayTag(FName("Character.Action.Movable.Attack"));
	ExhaustedTag = FGameplayTag::RequestGameplayTag(FName("Character.Status.Exhausted"));

	CurrentStamina = 100.f;
	MaxStamina = 100.f;
	StaminaRestoreAmount = 10.f; //초당 Stamina회복량
  SprintStaminaCost = 25.f; //Sprint 초당 소모량
  ParryStaminaCost = 2.5f; //Parry 한번당 소모량
	DodgeStaminaCost = 20.f; //Dodge 한번당 소모량
  bIsExhausted = false;
	//테스트 코드
	
	MaxHealth = 100.f;
	CurrentHealth = 100.f;

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
	TWeakObjectPtr<USanzoStatComponent> WeakThis(this); //약한 참조로 람다에서 this사용
  GetWorld()->GetTimerManager().SetTimer(
		StaminaRestoreHandle, 
		[WeakThis]() //인자(회복량)를 받아야 해서 람다로 변환
		{
      if (WeakThis.IsValid())
      {
        WeakThis->RestoreStamina(WeakThis->StaminaRestoreAmount * 0.01f);
        WeakThis->BroadCastStatUpdate();
      }
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
  GetWorld()->GetTimerManager().ClearTimer(ExhaustionRecoveryTimerHandle);
}


void USanzoStatComponent::RequestConsumeStaminaForSprint(bool bShouldConsume)
{
	bool bIsTimerActive = GetWorld()->GetTimerManager().IsTimerActive(SprintStaminaCostHandle);
	if(bShouldConsume == true && !bIsTimerActive) //true면서 타이머가없을때	소모
	{
		TWeakObjectPtr<USanzoStatComponent> WeakThis(this); //약한 참조로 람다에서 this사용
		GetWorld()->GetTimerManager().SetTimer(
			SprintStaminaCostHandle, 
			[WeakThis]()
			{
				if (WeakThis.IsValid())
				{
					WeakThis->ConsumeStamina(WeakThis->SprintStaminaCost * 0.01f);
					WeakThis->BroadCastStatUpdate();
				}
			},
			0.01f, 
      true);
	}
	else if(!bShouldConsume && bIsTimerActive)
	{
		GetWorld()->GetTimerManager().ClearTimer(SprintStaminaCostHandle);
	}
}

void USanzoStatComponent::ConsumeStaminaForAction(EActionType Type)
{
	switch (Type)
	{
	case EActionType::Parry:
		ConsumeStamina(ParryStaminaCost); // 2.5만큼 소모
		break;
	case EActionType::Dodge:
		ConsumeStamina(DodgeStaminaCost);
		break;

	default:
		break;

	}

	BroadCastStatUpdate();
}




void USanzoStatComponent::RestoreHealth(float Amount)
{
	CurrentHealth += Amount;
	if (CurrentHealth > MaxHealth)
	{
		CurrentHealth = MaxHealth;
	}
}

void USanzoStatComponent::AddExperience(float Amount)
{
	CurrentExp += Amount;
	if(CurrentExp >= 100)
	{
		CurrentExp -= 100;
		LevelUp();
	}
	BroadCastStatUpdate();
}

void USanzoStatComponent::LevelUp()
{
	Level++;
	if (APawn* SanzoPawn = Cast<APawn>(GetOwner()))
	{
		if (ASanzoPlayerController* PlayerController = Cast<ASanzoPlayerController>(SanzoPawn->GetController()))
		{
			//TODO : 소리 재생


			TWeakObjectPtr<ASanzoPlayerController> WeakPC(PlayerController); //약한 참조로
			TWeakObjectPtr<USanzoStatComponent> WeakThis(this); //약한 참조로
			GetWorld()->GetTimerManager().SetTimer(LevelUpLateHandle,
				[WeakPC,WeakThis]()
				{
					if(WeakPC.IsValid())
					{
						WeakPC->ShowPopUp(SanzoTags::UpgradeSelet);
					}
					if(WeakThis.IsValid())
					{
						WeakThis->AddExperience(0);
					}
				},
				0.8f,
				false
			);

			
				
		}
	}
	BroadCastStatUpdate();
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
    if(ISanzoTagEditorInterface* TagEditor = Cast<ISanzoTagEditorInterface>(GetOwner()))
		{
			TagEditor->AddGameplayTag(SanzoTags::Exhausted);
    	//HUD 업데이트 방송 - 작업자: 이준로
    	OnExhaustedChanged.Broadcast(true);
    }
		BeginExhaustionCooldown(); // 탈진 회복 시작
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

void USanzoStatComponent::ExhaustionRecovery()
{
	if (ISanzoTagEditorInterface* TagEditor = Cast<ISanzoTagEditorInterface>(GetOwner()))
	{
    TagEditor->RemoveGameplayTag(SanzoTags::Exhausted);
  }
}

void USanzoStatComponent::ApplyUpgrade(EUpgradeTarget Target, EUpgradeType Type, float Value)
{
  if (Target == EUpgradeTarget::Stat) //받은게 스탯일 경우에만 실행 
  {
    switch (Type)
    {
    case EUpgradeType::MaxHealth:
			MaxHealth += Value;
      break;
    case EUpgradeType::MaxStamina:
			MaxStamina += Value;
      break;
    default:
			GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("샤갈! 이상한값이 발생했어요!"));
      break;

    }
	}
}

void USanzoStatComponent::BeginExhaustionCooldown()
{
  if (GetWorld() && CheckTag(SanzoTags::Exhausted))
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(ExhaustionRecoveryTimerHandle))
		{
			return;
		}
			GetWorld()->GetTimerManager().SetTimer(
			ExhaustionRecoveryTimerHandle,
			this,
			&USanzoStatComponent::ExhaustionRecovery,
			3.f,
			false);
    
		
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
	Data.MaxHealth = MaxHealth;
	Data.CurrentHealth = CurrentHealth;
	Data.HealthPercent = (MaxHealth > 0.f) ? CurrentHealth / MaxHealth : 0.f;
	
	Data.MaxStamina = MaxStamina;
	Data.CurrentStamina = CurrentStamina;
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
#pragma region SaveLoad
FSanzoSaveStatData USanzoStatComponent::GetSaveData() const
{
	FSanzoSaveStatData Data;
	Data.Level = Level;
	Data.CurrentExp = CurrentExp;

	return Data;
}
void USanzoStatComponent::LoadFromSaveData(const FSanzoSaveStatData& SaveData)
{
	Level = SaveData.Level;
	CurrentExp = SaveData.CurrentExp;

	BroadCastStatUpdate(); // UI 갱신
}
#pragma endregion 최윤서
