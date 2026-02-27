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
	Legend
};

//업그레이드 타겟 Enum
UENUM(BlueprintType)
enum class EUpgradeTarget : uint8
{
	Character, //이동속도, 이쁨
	Stat, //체력, 스태미나
	Gun,
	Bow
};

//스탯 타입 Enum
UENUM(BlueprintType)
enum class EUpgradeType : uint8
{
	MaxHealth,
	MaxStamina,
	Beauty,
	Damage,
	FireRate, // 총 공격 속도
	MaxChargeTime, // 활 공격 속도
	Exp,
  Speed, // 이동 속도
  ParryReflectChance, // 패리 반사 확률
	Defense,            // 받는 피해 감소율
	LifeSteal,          // 공격 시 체력 회복 %
	DodgeChance        // 회피 확률 (이 존재하는가...를 모릅니다) -윤서
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
	
};

//두개의 변수 확인용 키 값 스트럭쳐
USTRUCT(BlueprintType)
struct FUpgradeStatKey
{
	GENERATED_BODY()
	
	EUpgradeTarget Target;
	EUpgradeType Type;
	
	FUpgradeStatKey() : Target(EUpgradeTarget::Character), Type(EUpgradeType::MaxHealth) {}
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