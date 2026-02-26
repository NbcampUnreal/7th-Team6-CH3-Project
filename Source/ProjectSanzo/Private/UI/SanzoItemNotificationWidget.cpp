// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SanzoItemNotificationWidget.h"

#include "Components/TextBlock.h"

void USanzoItemNotificationWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
}

void USanzoItemNotificationWidget::SetNotify(FName ItemType, int32 Amount)
{
	FNotifyTextInfo* NotifyTextInfo = ItemNameTable.Find(ItemType);
	
	FText DisplayInfo = FText::FromName(ItemType);
	FLinearColor DisplayColor = FLinearColor::White;
	
	if (NotifyTextInfo)
	{
		DisplayInfo = NotifyTextInfo->DisplayInfo;
		DisplayColor = NotifyTextInfo->DisplayColor;
	}
	
	FFormatNamedArguments Args;
	Args.Add(TEXT("ItemName"), DisplayInfo);
	Args.Add(TEXT("Amount"), FText::AsNumber(Amount));

	FText FinalMessage = FText::Format(
			FText::FromString(TEXT("{ItemName} +{Amount}")), 
			Args
	);
	
	if (ItemNotifyText)
	{
		ItemNotifyText->SetText(FinalMessage);
		ItemNotifyText->SetColorAndOpacity(FSlateColor(DisplayColor));
	}
	
	if (ItemNotifyAnim)
	{
		FWidgetAnimationDynamicEvent EndEvent;
		EndEvent.BindDynamic(this, &USanzoItemNotificationWidget::OnNotifyAnimFinished);
		BindToAnimationFinished(ItemNotifyAnim, EndEvent);
		
		PlayAnimation(ItemNotifyAnim);
	}	
}

void USanzoItemNotificationWidget::OnNotifyAnimFinished()
{
	RemoveFromParent();
}
