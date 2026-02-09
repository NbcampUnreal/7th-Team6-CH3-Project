// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/SanzoPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"

void ASanzoPlayerController::BeginPlay()
{
  Super::BeginPlay();

  if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
  {
    if (DefaultMappingContext)
    {
      Subsystem->AddMappingContext(DefaultMappingContext, 0);
    }
  }
}