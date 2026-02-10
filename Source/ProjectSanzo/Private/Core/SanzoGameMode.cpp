// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/SanzoGameMode.h"
#include "Core/SanzoGameState.h"
#include "Core/SanzoStageTypes.h"
#include "Character/SanzoCharacter.h"
#include "Character/SanzoPlayerController.h"
#include "UObject/ConstructorHelpers.h"

ASanzoGameMode::ASanzoGameMode()
{
    DefaultPawnClass = ASanzoCharacter::StaticClass();
    PlayerControllerClass = ASanzoPlayerController::StaticClass();
    GameStateClass = ASanzoGameState::StaticClass();
}

void ASanzoGameMode::BeginPlay()
{
  Super::BeginPlay();

  InitStageType();
}

void ASanzoGameMode::InitStageType()
{
  if (GetWorld()->GetMapName().Contains("Stage1"))
    CurrentStageType = ESanzoStageType::Extermination;
  else if (GetWorld()->GetMapName().Contains("Stage2"))
    CurrentStageType = ESanzoStageType::Survival;
  else if (GetWorld()->GetMapName().Contains("Stage3"))
    CurrentStageType = ESanzoStageType::Boss;
}
void ASanzoGameMode::StartStage()
{

}
void ASanzoGameMode::OnStageCleared()
{
  // 스테이지 이동
  UE_LOG(LogTemp, Warning, TEXT("GM: 스테이지 클리어"));
}
// 업그레이드 선택 트리거, 캐릭터에서 호출
void ASanzoGameMode::TriggerUpgradeSelection()
{
  UE_LOG(LogTemp, Warning, TEXT("GM: 캐릭터 성장 선택"));

  OnUpgradeSelected();
}

void ASanzoGameMode::OnUpgradeSelected()
{
  UE_LOG(LogTemp, Warning, TEXT("GM: 캐릭터 성장 선택 표시"));

}
