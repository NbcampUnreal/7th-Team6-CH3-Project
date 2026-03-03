// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SanzoBossOverheadWidget.h"
#include "AI/SanzoEnemyBase.h"
#include "Common/SanzoLog.h"
#include "Components/Image.h"

void USanzoBossOverheadWidget::UpdateOverHeadWidget(const FEnemyOverHeadData& Data)
{
	UpdateStunImage(Data.bIsStunned);
}

void USanzoBossOverheadWidget::UpdateStunImage(bool bIsStunned)
{
	if (StateImage)
	{
		if (bIsStunned)
		{
			if (!IsAnimationPlaying(StunStateAnim))
			{
				StateImage->SetOpacity(1.0f);
				PlayAnimation(StunStateAnim, 0, 0);
			}
		}
		else
		{
			if (IsAnimationPlaying(StunStateAnim))
			{
				StopAnimation(StunStateAnim);
				StateImage->SetOpacity(0.0f);
			}
		}
	}
}
