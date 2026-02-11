// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SanzoAIController.generated.h"
#include "Perception/AIPerceptionTypes.h"

class UAISenseConfig_Sight;

UCLASS()
class PROJECTSANZO_API ASanzoAIController : public AAIController
{
  GENERATED_BODY()

public:
  ASanzoAIController();

protected:
  virtual void OnPossess(APawn* InPawn) override;

#pragma region Perception
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
  TObjectPtr<class UAIPerceptionComponent> AIPerception;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
  TObjectPtr<UAISenseConfig_Sight> SightConfig;

  UFUNCTION()
  void OnTargetDetected(AActor* Actor, FAIStimulus Stimulus);
#pragma endregion

};
