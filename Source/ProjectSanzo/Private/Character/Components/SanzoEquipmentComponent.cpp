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

	//HUD가 처음 총알 정보 받아오는 부분 - 작업자: 이준로
	if (ASanzoGun* Gun = Cast<ASanzoGun>(CurrentWeapon))
	{
		Gun->OnAmmoChanged.AddDynamic(this, &USanzoEquipmentComponent::UpdateHUDAmmo);

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
	if (Target != EUpgradeTarget::Gun && Target != EUpgradeTarget::Bow)
	{
		return;
	}

	// 인벤토리 다 열어봐서 맞는 무기 찾기
	for (ASanzoWeaponBase* Weapon : Inventory)
	{
		if (!Weapon) continue;

		// 타겟이 총이고 꺼낸 무기가 총일 때 적용
		if (Target == EUpgradeTarget::Gun)
		{
			if (ASanzoGun* Gun = Cast<ASanzoGun>(Weapon))
			{
				Gun->ApplyWeaponStatUpgrade(Type, Value);
			}
		}
		// 타겟이 활이고 꺼낸 무기가 활일 때 적용
		else if (Target == EUpgradeTarget::Bow)
		{
			if (ASanzoBow* Bow = Cast<ASanzoBow>(Weapon))
			{
				Bow->ApplyWeaponStatUpgrade(Type, Value);
			}
		}
	}

}

void USanzoEquipmentComponent::AddAmmo(int32 Amount)
{
	if (ASanzoGun* Gun = Cast<ASanzoGun>(Inventory[0]))
	{
		Gun->AddAmmo(Amount); //weaponbase오버라이드시키고싶지만.. 시간이업다
	}
}


#pragma region UIDataTransfer



void USanzoEquipmentComponent::UpdateHUDAmmo()
{
	for (ASanzoWeaponBase* Weapon : Inventory)
	{
		if (ASanzoGun*Gun = Cast<ASanzoGun>(Weapon))
		{
			OnAmmoChanged.Broadcast(Gun->GetAmmoTextForHUD());
			return;
		}
	}
}

void USanzoEquipmentComponent::HandleWeaponHitEnemy()
{
	if (OnAnyWeaponHitEnemy.IsBound())
	{
		OnAnyWeaponHitEnemy.Broadcast();
	}
}

void USanzoEquipmentComponent::NotifyItemPickedUp(FName InItemType, int32 InAmount)
{
	if (OnItemPickedUp.IsBound())
	{
		OnItemPickedUp.Broadcast(InItemType, InAmount);
	}
}

float USanzoEquipmentComponent::GetGunDamage()
{
	if (ASanzoGun* Gun = Cast<ASanzoGun>(Inventory[0]))
	{
		return Gun->GetBaseDamage();
	}
	return 0;
}

float USanzoEquipmentComponent::GetGunFireRate()
{
	if (ASanzoGun* Gun = Cast<ASanzoGun>(Inventory[0]))
	{
		return Gun->GetFireRate();
	}
	return 0;
}

float USanzoEquipmentComponent::GetBowDamage()
{
	if (ASanzoBow* Bow = Cast<ASanzoBow>(Inventory[1]))
	{
		return Bow->GetBaseDamage();
	}
	return 0;
}

float USanzoEquipmentComponent::GetBowChargeTime()
{
	if (ASanzoBow* Bow = Cast<ASanzoBow>(Inventory[1]))
	{
		return Bow->GetMaxChargeTime();
	}
	return 0;
}

#pragma endregion 이준로

#pragma region 무기 스왑 로직 추가
void USanzoEquipmentComponent::SwapWeapon(bool bUpdateAnimInstance)
{
	// 인벤토리에 무기가 2개 이상일 때만 스왑 가능
	if (Inventory.Num() < 2) return;

	// 다음 무기 인덱스 계산
	int32 NextIndex = (CurrentWeaponIndex + 1) % Inventory.Num();
	// 해당 무기 장착
	EquipWeaponByIndex(NextIndex, bUpdateAnimInstance);
}

void USanzoEquipmentComponent::EquipWeaponByIndex(int32 Index, bool bUpdateAnimInstance)
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
	
	if (OnSwapped.IsBound())
	{
		OnSwapped.Broadcast(CurrentWeaponIndex);
	}

	// 바꾼 무기 보이도록 변경
	CurrentWeapon->SetActorHiddenInGame(false);

	// 무기에 따라 다르게 만들어둔 ABP로 교체
	if (bUpdateAnimInstance && CurrentWeapon->WeaponAnimInstanceClass)
	{
		OwnerCharacter->GetMesh()->SetAnimInstanceClass(CurrentWeapon->WeaponAnimInstanceClass);
	}
}

UAnimMontage* USanzoEquipmentComponent::BeginSwapWeapon()
{
	// 인벤토리에 무기가 2개 이상일 때만 진행
	if (Inventory.Num() < 2) return nullptr;

	// 다음에 꺼낼 무기 인덱스 계산
	int32 NextIndex = (CurrentWeaponIndex + 1) % Inventory.Num();
	ASanzoWeaponBase* NextWeapon = Inventory[NextIndex];

	if (NextWeapon && NextWeapon->EquipMontage)
	{
		if (ASanzoCharacter* Character = GetOwnerCharacter())
		{
			// 몽타지 틀기 전에 먼저 ABP 파일 교체
			if (NextWeapon->WeaponAnimInstanceClass)
			{
				Character->GetMesh()->SetAnimInstanceClass(NextWeapon->WeaponAnimInstanceClass);
			}
			// 교체된 애님인스턴스 가져와서 몽타지 재생
			if (UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance())
			{
				AnimInstance->Montage_Play(NextWeapon->EquipMontage,2.0f);
				AnimInstance->OnMontageEnded.RemoveDynamic(this, &USanzoEquipmentComponent::OnSwapMontageEnded);
				AnimInstance->OnMontageEnded.AddDynamic(this, &USanzoEquipmentComponent::OnSwapMontageEnded);
				return NextWeapon->EquipMontage;
			}
		}
	}
	return nullptr;
}

void USanzoEquipmentComponent::OnSwapMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (ASanzoCharacter* Character = Cast<ASanzoCharacter>(GetOwnerCharacter()))
	{
		// 감지되면 해제
		if (UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance())
		{
			AnimInstance->OnMontageEnded.RemoveDynamic(this, &USanzoEquipmentComponent::OnSwapMontageEnded);
		}

		// 스왑 중에 몽타지 취소되면
		if (bInterrupted && CurrentWeapon)
		{
			FTimerHandle RollbackTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(
				RollbackTimerHandle,
				FTimerDelegate::CreateWeakLambda(this, [Character, this]()
					{
						// 타이머가 도는 아주 짧은 찰나에 캐릭터나 무기가 파괴되었을 수 있으니 안전 검사
						if (Character && Character->GetMesh() && CurrentWeapon)
						{
							Character->GetMesh()->SetAnimInstanceClass(CurrentWeapon->WeaponAnimInstanceClass);
							Character->ForceResetState();
							Character->ApplyHitEffect();
					}),
				0.01f, // 0.01초 지연
				false
			);
		}
	}
}
#pragma endregion 이용호

