// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/SanzoUpgradeComponent.h"

//#include "Character/SanzoCharacter.h" // 제거필요
//#include "Character/Components/SanzoEquipmentComponent.h" //제거필요
//#include "Character/Components/SanzoStatComponent.h" //제거필요
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
  
	//여기까지

#pragma region ApplyUpgrade
	//캐릭터 업그레이드 적용
  if (ISanzoUpgradeInterface* UpgradeInterface = Cast<ISanzoUpgradeInterface>(Owner))
  {
    UpgradeInterface->ApplyUpgrade(Target, Type, Value);
  }

	//컴포넌트들(stat, Equip 의 업그레이드 적용)
  TArray<UActorComponent*> UpgradeComponents;
  UpgradeComponents = Owner->GetComponentsByInterface(USanzoUpgradeInterface::StaticClass());
  for (UActorComponent* Comp : UpgradeComponents)
  {
    if (ISanzoUpgradeInterface* Receiver = Cast<ISanzoUpgradeInterface>(Comp))
    {
      Receiver->ApplyUpgrade(Target, Type, Value);
    }
  }
#pragma endregion 김형백


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

