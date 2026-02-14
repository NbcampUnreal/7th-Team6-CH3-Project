// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UpgradeDataRow.generated.h"
/**
 * 
 */

USTRUCT(BlueprintType)
struct FUpgradeDataRow : public FTableRowBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName UpgradeID; // 고유 ID

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MinValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxValue;
};

UENUM(BlueprintType)
enum class EUpgradeRarity : uint8
{
	Common,
	Rare,
	Epic,
	Legend
};

USTRUCT(BlueprintType)
struct FUpgradeOption
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FName UpgradeID;

	UPROPERTY(BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly)
	float SelectedValue;
	
	UPROPERTY(BlueprintReadOnly)
	EUpgradeRarity Rarity;
};