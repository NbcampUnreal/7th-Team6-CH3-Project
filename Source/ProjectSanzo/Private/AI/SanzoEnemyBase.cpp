// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/SanzoEnemyBase.h"
#include "AI/SanzoAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

ASanzoEnemyBase::ASanzoEnemyBase()
{
  PrimaryActorTick.bCanEverTick = false;

  AIControllerClass = ASanzoAIController::StaticClass();
  // 맵에 놓거나 스폰되면 자동 빙의
  AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

  GetCharacterMovement()->MaxWalkSpeed = 350.f;
  // 이동 방향을 보도록 설정
  GetCharacterMovement()->bOrientRotationToMovement = true;

  GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);
}

void ASanzoEnemyBase::BeginPlay()
{
  Super::BeginPlay();
  CurrentHP = MaxHP;
}

//void ASanzoEnemyBase::Tick(float DeltaTime)
//{
//  Super::Tick(DeltaTime);
//
//}

void ASanzoEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
  Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool ASanzoEnemyBase::IsDead() const
{
  return CurrentHP <= 0.f;
}

