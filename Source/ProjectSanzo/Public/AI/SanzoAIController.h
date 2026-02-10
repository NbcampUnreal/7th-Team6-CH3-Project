// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SanzoAIController.generated.h"

UCLASS()
class PROJECTSANZO_API ASanzoAIController : public AAIController
{
  GENERATED_BODY()

public:
  ASanzoAIController();

protected:
  virtual void OnPossess(APawn* InPawn) override;

};
