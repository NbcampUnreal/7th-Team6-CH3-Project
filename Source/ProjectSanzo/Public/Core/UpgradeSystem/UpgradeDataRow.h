// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UpgradeDataRow.generated.h"

//레어도 Enum
UENUM(BlueprintType)
enum class EUpgradeRarity : uint8
{
	Common,
	Rare,
	Epic,
	Legend,
	Link
};

//업그레이드 타겟 Enum
UENUM(BlueprintType)
enum class EUpgradeTarget : uint8
{
	None,
	Character, //이동속도, 이쁨
	Stat, //체력, 스태미나
	Gun,
	Bow
};

//스탯 타입 Enum
UENUM(BlueprintType)
enum class EUpgradeType : uint8
{
	None,
	MaxHealth,
	MaxStamina,
	Beauty,
	Damage,
	FireRate, // 총 공격 속도
	MaxChargeTime, // 활 공격 속도
  Speed, // 이동 속도
  ParryReflectChance, // 패리 반사 확률
	HomingMissile, // (총 업그레이드) 하면 적을 따라가는 투사체 생성
	MultiShot, // 화살 분열
};

//UI용 모디파이어 Enum
UENUM(BlueprintType)
enum class EStatModifierType : uint8
{
	None,
	FlatPlus,
	FlatMinus,
	PercentPlus,
	PercentMinus,
	PercentMultiply,
	PercentDivide,
};

USTRUCT(BlueprintType)
struct FUpgradeDataRow : public FTableRowBase
{
	GENERATED_BODY()
	
public:
	//고유 ID
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName UpgradeID;
	//표시 이름
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DisplayName;
	//업그레이드 대상
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EUpgradeTarget UpgradeTarget;
	//업그레이드 타겟
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EUpgradeType UpgradeType;
	//옵션 값
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Value;
	//옵션 최대값
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float LimitValue;
	//레어도
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EUpgradeRarity Rarity;
	//스폰 가중치
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SpawnWeight;	
	//업그레이드 이미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> IconTexture;
	//업그레이드 UI 출력용 모디파이어 정보
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EStatModifierType ModifierType;
};

//선택된 업그레이드 옵션 Struct
USTRUCT(BlueprintType)
struct FUpgradeOption
{
	GENERATED_BODY()
	
public:
	//고유 ID
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName UpgradeID;
	//표시 이름
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DisplayName;
	//업그레이드 대상
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EUpgradeTarget UpgradeTarget;
	//업그레이드 타겟
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EUpgradeType UpgradeType;
	//옵션 값
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Value;
	//레어도
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EUpgradeRarity Rarity;
	//업그레이드 이미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> IconTexture;
	//업그레이드 UI 출력용 모디파이어 정보
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EStatModifierType ModifierType;	
};

//두개의 변수 확인용 키 값 스트럭쳐
USTRUCT(BlueprintType)
struct FUpgradeStatKey
{
	GENERATED_BODY()
	
	EUpgradeTarget Target;
	EUpgradeType Type;
	
	FUpgradeStatKey() : Target(EUpgradeTarget::None), Type(EUpgradeType::None) {}
	FUpgradeStatKey(EUpgradeTarget InputTarget, EUpgradeType InputType)	: Target(InputTarget), Type(InputType) {}
	
	bool operator==(const FUpgradeStatKey& Other) const
	{
		return Target == Other.Target && Type == Other.Type;
	}
	
	friend uint32 GetTypeHash(const FUpgradeStatKey& Key)
	{
		return HashCombine(GetTypeHash(Key.Target), GetTypeHash(Key.Type));
	}
	
};

//캐릭터 스테이터스 인포 전용 데이터 테이블
USTRUCT(BlueprintType)
struct FStatusDisplayData
{
	GENERATED_BODY()
	FStatusDisplayData()
		: DisplayTarget(EUpgradeTarget::None)
		  , UpgradeType(FUpgradeStatKey())
		  , DisplayText(FText::GetEmpty())
		  , BaseValue(0.0f)
		  , Value(0.0f)
			, ModifierType(EStatModifierType::None)
	{
	}

	FStatusDisplayData(EUpgradeTarget InTarget, FUpgradeStatKey InKey, FText InText, float InBaseValue, float InValue, EStatModifierType InModifierType)
		: DisplayTarget(InTarget)
		  , UpgradeType(InKey)
		  , DisplayText(InText)
		  , BaseValue(InBaseValue)
		  , Value(InValue)
			, ModifierType(InModifierType)
	{
	}
	
	UPROPERTY()
	EUpgradeTarget DisplayTarget;
	
	UPROPERTY()
	FUpgradeStatKey UpgradeType;

	UPROPERTY()
	FText DisplayText;
	
	UPROPERTY()
	float BaseValue;

	UPROPERTY()
	float Value;
	
	UPROPERTY()
	EStatModifierType ModifierType;
	
};