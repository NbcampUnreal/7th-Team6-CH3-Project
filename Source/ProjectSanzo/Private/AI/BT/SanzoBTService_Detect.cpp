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

  UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
  AActor* Target = Cast<AActor>(Blackboard->GetValueAsObject(TEXT("TargetActor")));

  if (Target)
  {
    // 타겟 발견 시 -> 달리기
    AIChar->GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;

    float Distance = FVector::Dist(
      ControllingPawn->GetActorLocation(),
      Target->GetActorLocation()
    );

    Blackboard->SetValueAsFloat(TEXT("DistanceToTarget"), Distance);

    if (Distance > 1500.0f)
    {
      Blackboard->SetValueAsObject(TEXT("TargetActor"), nullptr);
      Blackboard->SetValueAsFloat(TEXT("DistanceToTarget"), 99999.f);
    }
  }
  else
  {
    // 타겟 없음 -> 걷기
    AIChar->GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
  }
}

