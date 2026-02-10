// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SanzoAIController.h"
#include "AI/SanzoEnemyBase.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

ASanzoAIController::ASanzoAIController()
{
}

void ASanzoAIController::OnPossess(APawn* InPawn)
{
  Super::OnPossess(InPawn);

  if (ASanzoEnemyBase* Enemy = Cast<ASanzoEnemyBase>(InPawn)) {
    if (Enemy->EnemyBehaviorTree) {
      RunBehaviorTree(Enemy->EnemyBehaviorTree);
    }
  }
}
