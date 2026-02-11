// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SanzoHUDWidget.generated.h"

UCLASS()
class PROJECTSANZO_API USanzoHUDWidget : public UUserWidget
{
	GENERATED_BODY()
#pragma region UI

public:
	//초기 생성 로직
	virtual void NativeConstruct() override;

	//Stat Component 로 부터 받아올 정보 위젯 연결
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ExpBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* StaminaBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LevelText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AmmoCountText;

	UPROPERTY(meta = (BindWidget))
	class UImage* WeaponImage;
	//추후 함수 수정 필요
	void UpdateHUD();

	//Game State로 부터 받아올 정보 연결
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* StageText;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* StageProgressBar;

	UFUNCTION()
	void UpdateStageProgressBar();


#pragma endregion 이준로
};
