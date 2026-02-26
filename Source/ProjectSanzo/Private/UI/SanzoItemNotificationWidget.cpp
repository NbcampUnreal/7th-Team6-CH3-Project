// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SanzoItemNotificationWidget.h"

#include "Components/TextBlock.h"

void USanzoItemNotificationWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (ItemNotifyAnim)
	{
		FWidgetAnimationDynamicEvent EndEvent;
		EndEvent.BindDynamic(this, &USanzoItemNotificationWidget::OnNotifyAnimFinished);
		BindToAnimationFinished(ItemNotifyAnim, EndEvent);
	}
	
}

void USanzoItemNotificationWidget::SetNotify(FName ItemType, int32 Amount)
{
	// 아이템 베이스 업데이트 시 추가
	
}

void USanzoItemNotificationWidget::OnNotifyAnimFinished()
{
}
