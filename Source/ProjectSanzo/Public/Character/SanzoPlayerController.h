// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SanzoPlayerController.generated.h"

class UInputMappingContext;

UCLASS(abstract)
class PROJECTSANZO_API ASanzoPlayerController : public APlayerController
{
  GENERATED_BODY()
protected:
  UPROPERTY(EditAnywhere, Category = "Input")
  TObjectPtr<UInputMappingContext> DefaultMappingContext;

  virtual void BeginPlay() override;
};
