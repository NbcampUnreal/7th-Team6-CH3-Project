#pragma once

#include "CoreMinimal.h"
#include "SanzoBaseValue.generated.h"

USTRUCT(BlueprintType)
struct FCharacterBaseValues
{
	GENERATED_BODY()
	
	static constexpr float MaxHealth = 100.0f;
	
	static constexpr float MaxStamina = 100.0f;
	
	static constexpr float MoveSpeed = 500.0f;
	
	static constexpr float FaceLevel = 0;
	
	static constexpr float ParryReflectChance = 50.0f;

};

USTRUCT(BlueprintType)
struct FGunBaseValues
{
	GENERATED_BODY()
	
	static constexpr float BaseDamage = 8;
	
	static constexpr float FireRate = 0.15f;
	
	static constexpr float HomingMissileChance = 0;
};

USTRUCT(BlueprintType)
struct FBowBaseValues
{
	GENERATED_BODY()
	
	static constexpr float BaseDamage = 40;
	
	static constexpr float MaxChargeTime = 1;
	
	static constexpr float MultiShot = 0;
};