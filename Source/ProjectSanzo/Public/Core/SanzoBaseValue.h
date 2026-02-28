#pragma once

#include "CoreMinimal.h"
#include "SanzoBaseValue.generated.h"

USTRUCT(BlueprintType)
struct FCharacterBaseValues
{
	GENERATED_BODY()
	
	static constexpr float MaxHealth = 100.0f;
	
	static constexpr float MaxStamina = 100.0f;
	
	static constexpr float DefaultMoveSpeed = 500.0f;

};

USTRUCT(BlueprintType)
struct FGunBaseValues
{
	GENERATED_BODY()
	
	static constexpr float BaseDamage = 5;
	
	static constexpr float BaseFireRate = 0.15f;
};

USTRUCT(BlueprintType)
struct FBowBaseValues
{
	GENERATED_BODY()
	
	static constexpr float BaseDamage = 40;
	
	static constexpr float BaseMaxChargeTime = 1;
};