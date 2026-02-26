#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SanzoItemNotificationWidget.generated.h"

USTRUCT(BlueprintType)
struct FNotifyTextInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	FText DisplayInfo;
	
	UPROPERTY(EditDefaultsOnly)
	FLinearColor DisplayColor;
	
	FNotifyTextInfo() 
	:	DisplayInfo(FText::GetEmpty()), DisplayColor(FLinearColor::White) {}
	
};


UCLASS()
class PROJECTSANZO_API USanzoItemNotificationWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
	void SetNotify(FName ItemType, int32 Amount);
	
	UFUNCTION()
	void OnNotifyAnimFinished();
	
protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemNotifyText;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* ItemNotifyAnim;
	
	UPROPERTY(EditDefaultsOnly, Category="UI|Settings")
	TMap<FName,FNotifyTextInfo> ItemNameTable;
	
};
