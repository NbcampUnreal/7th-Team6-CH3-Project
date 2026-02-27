#pragma once

#include "CoreMinimal.h"
#include "Core/SanzoStageTypes.h"
#include "Blueprint/UserWidget.h"
#include "SanzoStageInfoWidget.generated.h"

class UImage;

UCLASS()
class PROJECTSANZO_API USanzoStageInfoWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintCallable)
	void SetStageInfo(ESanzoStageType StageType);
	
protected:
	UFUNCTION()
	void SetDefaultStageInfo();
	
	UFUNCTION()
	void SetBossStageInfo();
	
	UFUNCTION()
	void HandleStageProgressChanged(float Percent);
	
	UFUNCTION()
	void HandleBossHealthChanged(float Percent);
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* StageInfoText;
	
	UPROPERTY(meta = (BindWidget))
	class USizeBox* StageProgressBarSizeBox;
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* StageProgressBar;
	
	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* StunGageBox;
	
	UPROPERTY()
	TArray<UImage*> StunImages;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Resources")
	class UTexture2D* FullTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Resources")
	class UTexture2D* EmptyTexture;
	
private:
	FString GetRandomBossName();
};
