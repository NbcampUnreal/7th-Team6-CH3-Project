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
  void SetStunGage(int32 MaxStunCount);
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateStunGage(int32 CurrentStunCount);
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateHealthBar(float HealthPercent);
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateStateImage(bool bIsStunned, bool bIsSighted);
	
protected:
	UPROPERTY(meta = (BindWidget))
	class UBorder* HealthBorder;
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;
	
	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* StunGageBox;
	
  UPROPERTY()
  UImage* StunImage;
  
	UPROPERTY()
	TArray<UImage*> StunGageImages;
	
	UPROPERTY(meta = (BindWidget))
	class UImage* StateImage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Resources")
	class UTexture2D* FullTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Resources")
	class UTexture2D* EmptyTexture;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Resources")
	class UTexture2D* FindTexture;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Resources")
	class UTexture2D* MissingTexture;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Resources")
	class UTexture2D* StunTexture;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* FindStateAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* MissingStateAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* StunStateAnim;
	
private:
  int32 CurrentMaxStunCount = 0;
};
