// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SanzoEnemyOverHeadWidget.generated.h"

struct FEnemyOverHeadData;
class UImage;

UCLASS()
class PROJECTSANZO_API USanzoEnemyOverHeadWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void UpdateOverHeadWidget(const FEnemyOverHeadData& OverHeadData);
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateStunGage(int32 CurrentStunCount);
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdatePerceptionMark(bool bIsSight);
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateHealthBar(float HealthPercent);
	
	
protected:
	UPROPERTY(meta = (BindWidget))
	class UBorder* HealthBorder;
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PerceptionMark;
	
	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* StunGageBox;
	
	UPROPERTY()
	TArray<UImage*> StunImages;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Resources")
	class UTexture2D* FullTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Resources")
	class UTexture2D* EmptyTexture;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* PerceptionMarkStartAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* PerceptionMarkBlinkAnim;
	
};
