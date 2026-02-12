// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SanzoHUDWidget.generated.h"

struct FSanzoStatData;

UCLASS()
class PROJECTSANZO_API USanzoHUDWidget : public UUserWidget
{
	GENERATED_BODY()
#pragma region UI

protected:
	//초기 생성 로직
	virtual void NativeConstruct() override;

	UFUNCTION()
	void HandleStatChanged(const FSanzoStatData& Data);

	void UpdateStageProgressBar();

public:
	//Stat Component 로 부터 받아올 정보 위젯 연결
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ExpBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LevelText;


	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* StaminaBar;

	// Weapon Component 로 부터 받아올 정보 위젯 연결
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AmmoCountText;

	UPROPERTY(meta = (BindWidget))
	class UImage* WeaponImage;

	//Game State로 부터 받아올 정보 위젯 연결
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* StageText;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* StageProgressBar;

#pragma endregion 이준로
};
