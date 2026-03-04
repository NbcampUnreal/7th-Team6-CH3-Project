// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/UpgradeSystem/UpgradeDataRow.h"
#include "SanzoUpgradeButtonWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpgradeButtonClicked, const FUpgradeOption&, SelectedUpgrade);

UCLASS()
class PROJECTSANZO_API USanzoUpgradeButtonWidget : public UUserWidget
{
	GENERATED_BODY()
#pragma region UpgradeButton
public:
	void SetUpgradeButton(const FUpgradeOption& InputOption);
	
	FOnUpgradeButtonClicked OnUpgradeButtonClicked;
	
protected:
	
	void SetUpgradeButtonText(FText InDisplayName, EStatModifierType InModifierType, float InValue);
	
	UPROPERTY(meta = (BindWidget))
	class UImage* UpgradeBackground;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* UpgradeButton;
	
	UPROPERTY(meta = (BindWidget))
	class UImage* UpgradeIcon;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* UpgradeText;
	
	FUpgradeOption CachedOption;
	
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void HandleButtonClicked();
	
	UTexture2D* GetTextureByRarity(EUpgradeRarity Rarity);

#pragma endregion 이준로
	
#pragma region ChangeTextureAndSoundByRarity
	
	UPROPERTY(EditAnywhere, Category= "UI|Style")
	TMap<EUpgradeRarity, UTexture2D*> RarityTextures;

#pragma endregion 이준로
	
#pragma region PlayButtonSoundByRarity
public:
	UFUNCTION(BlueprintCallable, Category = "PlayButtonSoundByRarity")
	void PlaySoundForDuration(float Duration);
	
	UFUNCTION(BlueprintCallable, Category = "PlayButtonSoundByRarity")
	void StopCurrentSound();
	
	
	UFUNCTION(BlueprintCallable, Category = "PlayButtonSoundByRarity")
	USoundBase* GetCurrentSound() { return CurrentSound; }
	
protected:
	
	FTimerHandle SoundStopTimerHandle;
	
	UPROPERTY()
	class UAudioComponent* CurrentAudioComponent;
	
	UPROPERTY()
	USoundBase* CurrentSound;
	
	UPROPERTY(EditAnywhere, Category= "UI|Sound")
	TMap<EUpgradeRarity, USoundBase*> RaritySounds;
	
	UFUNCTION()
	void SetSoundByRarity(EUpgradeRarity Rarity);
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
private:
	float SoundStopTargetRealTime =0.0f;
	
	bool bIsSoundDurationEnded = false;
	
#pragma endregion 
};
