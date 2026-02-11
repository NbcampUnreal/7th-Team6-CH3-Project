// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/BT/SanzoBTService_Detect.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

USanzoBTService_Detect::USanzoBTService_Detect()
{
  NodeName = TEXT("Detect & Speed Control");
  Interval = 0.5f; // 0.5초마다 실행 (최적화)
}

void USanzoBTService_Detect::TickNode(
  UBehaviorTreeComponent& OwnerComp,
  uint8* NodeMemory,
  float DeltaSeconds)
{
  Super::TickNode(
    OwnerComp,
    NodeMemory,
    DeltaSeconds);

  APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
  if (!ControllingPawn) return;

  ACharacter* AIChar = Cast<ACharacter>(ControllingPawn);
  if (!AIChar) return;

  UObject* TargetObject = OwnerComp.GetBlackboardComponent()
    ->GetValueAsObject(TEXT("TargetActor"));

  if (TargetObject)
  {
    // 타겟 발견 시 -> 달리기
    AIChar->GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
  }
  else
  {
    // 타겟 없음 -> 걷기
    AIChar->GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
  }
}

