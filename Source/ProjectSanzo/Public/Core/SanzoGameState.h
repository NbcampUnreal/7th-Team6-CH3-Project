// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SanzoGameState.generated.h"

UCLASS()
class PROJECTSANZO_API ASanzoGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	//임시 코드
	//ASanzoGameState();
	
	virtual void BeginPlay() override;
	

#pragma region UI
	
	FTimerHandle HUDUpdateTimerHandle;
	void UpdateHUD();

	//임시 함수
	//void StartStage();


#pragma endregion 이준로
};
