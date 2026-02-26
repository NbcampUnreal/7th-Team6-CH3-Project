// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/SanzoEquipmentComponent.h"
#include "Character/SanzoCharacter.h"
#include "Weapon/SanzoBow.h"
#include "Weapon/SanzoGun.h"
#include "Weapon/SanzoWeaponBase.h" //SanzoBow, SanzoGun 으로 3번 참조, Pragmaonce때문에 충돌 날 수있으니(불러와야하는데 못불러오는등) 수정예정

USanzoEquipmentComponent::USanzoEquipmentComponent()
{
  

	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


void USanzoEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwnerCharacter();
	SpawnParams.Instigator = GetOwnerCharacter();
	FVector SpawnLocation = FVector::ZeroVector;
	FRotator SpawnRotation = FRotator::ZeroRotator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

#pragma region 무기 스폰 로직 변경
	// 배열에 등록된 모든 무기 클래스를 미리 스폰
	for (TSubclassOf<ASanzoWeaponBase> WeaponClass : DefaultWeaponClasses)
	{
		if (WeaponClass)
		{
			ASanzoWeaponBase* SpawnedWeapon = GetWorld()->SpawnActor<ASanzoWeaponBase>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
			if (SpawnedWeapon)
			{
				SpawnedWeapon->AttachToComponent(
					OwnerCharacter->GetMesh(),
					FAttachmentTransformRules::SnapToTargetIncludingScale,
					SpawnedWeapon->AttachSocketName
				);

				// 스폰된 무기 전부 안보이게 변경
				SpawnedWeapon->SetActorHiddenInGame(true);
				Inventory.Add(SpawnedWeapon);
				
				// 적 타격시 HUD 이펙트 재생용 Broadcast - 작업자: 이준로
				SpawnedWeapon->OnEnemyHit.AddDynamic(this, &USanzoEquipmentComponent::HandleWeaponHitEnemy);
			}
		}
	}

	// 기본적으로 0번 인덱스에 있는 무기 꺼내기
	if (Inventory.Num() > 0)
	{
		EquipWeaponByIndex(0);
	}

	if (ASanzoGun* Gun = Cast<ASanzoGun>(CurrentWeapon))
	{
		Gun->OnAmmoChanged.AddDynamic(this, &USanzoEquipmentComponent::UpdateHUDAmmo);

		UpdateHUDAmmo();
	}

	if (ASanzoBow* Bow = Cast<ASanzoBow>(CurrentWeapon))
	{
		UpdateHUDAmmo();
	}
#pragma endregion 이용호
}


void USanzoEquipmentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

ASanzoCharacter* USanzoEquipmentComponent::GetOwnerCharacter()
{
	if (OwnerCharacter == nullptr)
	{
		OwnerCharacter = Cast<ASanzoCharacter>(GetOwner());
  }	

	return OwnerCharacter;
}

void USanzoEquipmentComponent::ApplyUpgrade(EUpgradeTarget Target, EUpgradeType Type, float Value)
{
	if (Target == EUpgradeTarget::Gun) //받은게 총일경우에만 실행
	{
		CurrentWeapon->ApplyWeaponStatUpgrade(Type, Value);
	}
	if (Target == EUpgradeTarget::Bow) //받은게 보~우일경우에만 싱행
	{
		CurrentWeapon->ApplyWeaponStatUpgrade(Type, Value);
	}
}

#pragma region UIDataTransfer

void USanzoEquipmentComponent::UpdateHUDAmmo()
{
	if (CurrentWeapon)
	{
		OnAmmoChanged.Broadcast(CurrentWeapon->GetAmmoTextForHUD());
	}
}

void USanzoEquipmentComponent::HandleWeaponHitEnemy()
{
	if (OnAnyWeaponHitEnemy.IsBound())
	{
		OnAnyWeaponHitEnemy.Broadcast();
	}
}

#pragma endregion 이준로

#pragma region 무기 스왑 로직 추가
void USanzoEquipmentComponent::SwapWeapon()
{
	// 인벤토리에 무기가 2개 이상일 때만 스왑 가능
	if (Inventory.Num() < 2) return;

	// 다음 무기 인덱스 계산
	int32 NextIndex = (CurrentWeaponIndex + 1) % Inventory.Num();
	// 해당 무기 장착
	EquipWeaponByIndex(NextIndex);
}

void USanzoEquipmentComponent::EquipWeaponByIndex(int32 Index)
{
	if (!Inventory.IsValidIndex(Index)) return;


	if (CurrentWeapon)
	{
		// 기존에 끼고 있던 무기 안보이게 변경
		CurrentWeapon->SetActorHiddenInGame(true);
	}

	// 다른 무기로 변경
	CurrentWeaponIndex = Index;
	CurrentWeapon = Inventory[CurrentWeaponIndex];

	// 바꾼 무기 보이도록 변경
	CurrentWeapon->SetActorHiddenInGame(false);

	// 무기에 따라 다르게 만들어둔 ABP로 교체
	if (OwnerCharacter && OwnerCharacter->GetMesh())
	{
		if (CurrentWeapon->WeaponAnimInstanceClass)
		{
			// 해당 무기에 설정된 ABP로 ABP 파일 교체
			OwnerCharacter->GetMesh()->SetAnimInstanceClass(CurrentWeapon->WeaponAnimInstanceClass);
		}
	}
	
	if (OnSwapped.IsBound())
	{
		OnSwapped.Broadcast(CurrentWeaponIndex);
	}
	
}
#pragma endregion 이용호

