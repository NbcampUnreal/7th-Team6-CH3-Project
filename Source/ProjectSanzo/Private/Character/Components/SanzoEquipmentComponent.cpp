// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/SanzoEquipmentComponent.h"
#include "Character/SanzoCharacter.h"
#include "Weapon/SanzoWeaponBase.h"

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

	CurrentWeapon = GetWorld()->SpawnActor<ASanzoWeaponBase>(WeaponClass, SpawnLocation, SpawnRotation, SpawnParams);

	if(CurrentWeapon)
	{
		CurrentWeapon->AttachToComponent(
			GetOwnerCharacter()->GetMesh(),
			FAttachmentTransformRules::SnapToTargetIncludingScale,
			TEXT("HandGrip_R")
		);
	}
	
	
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

