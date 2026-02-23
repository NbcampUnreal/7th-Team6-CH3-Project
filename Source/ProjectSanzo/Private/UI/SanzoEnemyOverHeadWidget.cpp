// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SanzoEnemyOverHeadWidget.h"

#include "AI/SanzoEnemyBase.h"
#include "Common/SanzoLog.h"
#include "Components/Border.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void USanzoEnemyOverHeadWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (HealthBorder)
	{
		HealthBorder->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (PerceptionMark)
	{
		PerceptionMark->SetRenderOpacity(0.f);
	}
	
	if (StunGageBox)
	{
		StunImages.Empty();
		
		for (int i = 0; i < StunGageBox->GetChildrenCount(); i++)
		{
			if (UImage* ChildImage = Cast<UImage>(StunGageBox->GetChildAt(i)))
			{
				StunImages.Add(ChildImage);
			}
		}
	}
	
}

void USanzoEnemyOverHeadWidget::UpdateOverHeadWidget(const FEnemyOverHeadData& OverHeadData)
{
	UE_LOG(LogLJR, Warning, TEXT("위젯 수신 완료: bIsSighted = %s"), OverHeadData.bIsSighted ? TEXT("true") : TEXT("false"));
	UE_LOG(LogLJR, Warning, TEXT("위젯 수신 완료: CurrentStunCount = %d"), OverHeadData.CurrentStunCount);
	UpdateHealthBar(OverHeadData.HealthPercent);
	UpdateStunGage(OverHeadData.CurrentStunCount);
	UpdatePerceptionMark(OverHeadData.bIsSighted);
}

void USanzoEnemyOverHeadWidget::UpdateStunGage(int32 CurrentStunCount)
{
	if (StunGageBox)
	{
		StunGageBox->SetVisibility(ESlateVisibility::Visible);
		if (HealthBorder)
		{
			HealthBorder->SetVisibility(ESlateVisibility::Visible);
		}
		
		for (int i = 0; i < StunImages.Num(); i++)
		{
			if (StunImages[i])
			{
				UTexture2D* TargetTexture = (i < CurrentStunCount) ? EmptyTexture : FullTexture;
				StunImages[i]->SetBrushSize(FVector2D(50,50));
				StunImages[i]->SetBrushFromTexture(TargetTexture);
				
			}
		}
	}
}

void USanzoEnemyOverHeadWidget::UpdatePerceptionMark(bool bIsSight)
{
	StopAnimation(PerceptionMarkStartAnim);
	StopAnimation(PerceptionMarkBlinkAnim);
	
	if (PerceptionMark)
	{
		if (bIsSight)
		{
			if (PerceptionMarkStartAnim)
			{
				UE_LOG(LogLJR, Warning, TEXT("느낌표 표시 실행"));
				PerceptionMark->SetText(FText::FromString(FString::Printf(TEXT("!"))));
				PerceptionMark->SetColorAndOpacity(FLinearColor::Red);
				PlayAnimation(PerceptionMarkStartAnim);
			}
		}
		else
		{
			if (PerceptionMarkBlinkAnim)
			{
				UE_LOG(LogLJR, Warning, TEXT("물음표 표시 실행"));
				PerceptionMark->SetText(FText::FromString(FString::Printf(TEXT("?"))));
				PerceptionMark->SetColorAndOpacity(FLinearColor::Yellow);
				PlayAnimation(PerceptionMarkBlinkAnim);
			}
		}
	}
}

void USanzoEnemyOverHeadWidget::UpdateHealthBar(float HealthPercent)
{		
	if (HealthBar)
	{
		if (HealthPercent >= 1.f)
		{
			HealthBorder->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			HealthBorder->SetVisibility(ESlateVisibility::Visible);
			if (StunGageBox)
			{
				StunGageBox->SetVisibility(ESlateVisibility::Visible);
			}
		}
		UE_LOG(LogLJR,Warning,TEXT("HealthBarUpdate"));
		HealthBar->SetPercent(HealthPercent);
	}
}
