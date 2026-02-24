// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/SanzoUpgradeComponent.h"

#include "Character/SanzoCharacter.h"
#include "Character/Components/SanzoEquipmentComponent.h"
#include "Character/Components/SanzoStatComponent.h"
#include "Common/SanzoLog.h"
#include "Core/UpgradeSystem/SanzoUpgradeSubsystem.h"

// Sets default values for this component's properties
USanzoUpgradeComponent::USanzoUpgradeComponent()
{
}


// Called when the game starts
void USanzoUpgradeComponent::BeginPlay()
{
	Super::BeginPlay();

	USanzoUpgradeSubsystem* UpgradeSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USanzoUpgradeSubsystem>();
	if (UpgradeSubsystem)
	{
		UpgradeSubsystem->OnUpgradeSelected.AddUObject(this, &USanzoUpgradeComponent::OnNewUpgradeSelected);
		
		InitializeUpgradeFromSubsystem(UpgradeSubsystem->GetUpgradeTotalMap());
	}
	
}

void USanzoUpgradeComponent::OnNewUpgradeSelected(const FUpgradeOption& UpgradeOption)
{
	ProcessUpgradeValue(UpgradeOption.UpgradeTarget, UpgradeOption.UpgradeType, UpgradeOption.Value);
}

void USanzoUpgradeComponent::ProcessUpgradeValue(EUpgradeTarget Target, EUpgradeType Type, float Value)
{
	AActor* Owner = GetOwner();
	if (!Owner) return;
	
	//로그용 임시함수
	FString TargetStr = UEnum::GetValueAsString(Target);
	FString TypeStr = UEnum::GetValueAsString(Type);
	
	UE_LOG(LogLJR, Warning, TEXT("[Upgrade] Target: %s, Type: %s, Value: %.2f"), *TargetStr, *TypeStr, Value);
	//여기까지
	
	if (Target == EUpgradeTarget::Character)
	{
		if (Type == EUpgradeType::Beauty)
		{
			if (ASanzoCharacter* Character = Cast<ASanzoCharacter>(Owner))
			{
				UE_LOG(LogLJR, Warning, TEXT("외모 업그레이드 전달 완료"))
				//외모 업그레이드 함수 등록
				//Character->ApplyVisualUpgrade(Value);
			}
		}
		else
		{
			if (auto* StatComponent = Owner->FindComponentByClass<USanzoStatComponent>())
			{
				UE_LOG(LogLJR, Warning, TEXT("스탯 업그레이드 전달 완료"))
				//스탯 적용 함수 등록
				//StatComponent->ApplyStatUpgrade(Type, Value);
			}
		}
	}
	
	else if (Target == EUpgradeTarget::Gun || Target ==EUpgradeTarget::Bow)
	{
		if (auto* EquipmentComponent = Owner->FindComponentByClass<USanzoEquipmentComponent>())
		{
			UE_LOG(LogLJR, Warning, TEXT("%s, %s, %.2f 전달 완료"),*TargetStr,*TypeStr, Value);
			//장비 업그레이드 적용 함수 등록
			//EquipmentComponent->ApplyWeaponUpgrade(Target,Type,Value);
		}
	}
}

void USanzoUpgradeComponent::InitializeUpgradeFromSubsystem(const TMap<FUpgradeStatKey, float> TotalMap)
{
	for (const auto& Elem : TotalMap)
	{
		const FUpgradeStatKey& Key = Elem.Key;
		const float TotalValue = Elem.Value;
		
		ProcessUpgradeValue(Key.Target, Key.Type, TotalValue);
	}
}

