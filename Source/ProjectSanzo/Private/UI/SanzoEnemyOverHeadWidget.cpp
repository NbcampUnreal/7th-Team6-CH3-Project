// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SanzoEnemyOverHeadWidget.h"

#include "EditorDirectories.h"
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
	
	if (StunGageBox)
	{
		StunGageImages.Empty();
		
		for (int i = 0; i < StunGageBox->GetChildrenCount(); i++)
		{
			if (UImage* ChildImage = Cast<UImage>(StunGageBox->GetChildAt(i)))
			{
				StunGageImages.Add(ChildImage);
			}
		}
	}
	
}

void USanzoEnemyOverHeadWidget::UpdateOverHeadWidget(const FEnemyOverHeadData& OverHeadData)
{
	UpdateHealthBar(OverHeadData.HealthPercent);
	UpdateStunGage(OverHeadData.CurrentStunCount);
	UpdateStateImage(OverHeadData.bIsStunned, OverHeadData.bIsSighted);
	//UpdatePerceptionMark(OverHeadData.bIsSighted);
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
		
		for (int i = 0; i < StunGageImages.Num(); i++)
		{
			if (StunGageImages[i])
			{
				UTexture2D* TargetTexture = (i < CurrentStunCount) ? EmptyTexture : FullTexture;
				
				StunGageImages[i]->SetBrushFromTexture(TargetTexture);
				StunGageImages[i]->SetDesiredSizeOverride(FVector2D(50.0f, 50.0f));
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

void USanzoEnemyOverHeadWidget::UpdateStateImage(bool bIsStunned, bool bIsSight)
{
	StopAnimation(FindStateAnim);
	StopAnimation(MissingStateAnim);
	
	if (bIsStunned)
	{
		if (StateImage)
		{
			StateImage-> SetBrushFromTexture(StunTexture);
			StateImage->SetColorAndOpacity(FLinearColor::Gray);
			PlayAnimation(StunStateAnim,0,0);
		}
	}
	else
	{
		StopAnimation(StunStateAnim);
		if (StateImage)
		{
			if (bIsSight)
			{
				StateImage->SetBrushFromTexture(FindTexture);
				StateImage->SetColorAndOpacity(FLinearColor::Red);
				if (FindStateAnim)
				{
					PlayAnimation(FindStateAnim);
				}
			}
			else
			{
				StateImage->SetBrushFromTexture(MissingTexture);
				StateImage->SetColorAndOpacity(FLinearColor::Yellow);
				if (MissingStateAnim)
				{
					PlayAnimation(MissingStateAnim);
				}
			}
		}
	}
}
