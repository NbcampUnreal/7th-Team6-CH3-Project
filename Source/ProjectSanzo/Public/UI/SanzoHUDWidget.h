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

public:
	//초기 생성 로직
	virtual void NativeConstruct() override;

	UFUNCTION()
	void HandleStatChanged(const FSanzoStatData& Data);
	
	UFUNCTION()
	void HandleAmmoChanged(FText NewAmmoText);
	
	UFUNCTION()
	void HandleWeaponSwapped(int32 CurrentWeaponIndex);
	
	UFUNCTION()
	void UpdateBowChargingProgress(float NewPercent);
	
	UFUNCTION()
	void HandleEnemyHitAnim();

protected:
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
	class UOverlay* BowInfoOverlay;
	
	UPROPERTY(meta = (BindWidget))
	class UOverlay* GunInfoOverlay;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* GunAmmoCount;
	
	UPROPERTY(meta = (BindWidget))
	class UImage* BowAimProgressBar;
	
	UPROPERTY()
	UMaterialInstanceDynamic* BowAimProgressBarDynamic;
	
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* GunInfoSwapBackAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* HitEffectAnim;

#pragma endregion 이준로
	
#pragma region StaminaBarDynamic
public:
	
	UFUNCTION()
	void HandleStaminaColorChange(bool bNewExhausted);
	
	void UpdateStaminaColor(float DeltaTime);
	
protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	float ExhaustionStartTime;
	const float ExhaustionDuration = 3.0f;
	
	const FLinearColor NormalStaminaColor = FLinearColor(0.0f, 1.0f, 0.17f);
	const FLinearColor ExhaustedColor = FLinearColor::Gray;
	
	bool bIsExhausted = false;
	
#pragma endregion 이준로
	
#pragma region ItemNotification
	
public:
	UFUNCTION()
	void HandleItemNotification(FName ItemType, int32 Amount);
	
protected:
	UPROPERTY(EditAnywhere, Category= "UI")
	TSubclassOf<class USanzoItemNotificationWidget> ItemNotificationClass;
	
	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* NotificationContainer;
	
#pragma endregion 이준로
	
#pragma region StageInfo
	
public:	
	UPROPERTY(meta = (BindWidget))
	class USanzoStageInfoWidget* StageInfoWidget;

#pragma endregion 이준로
};
