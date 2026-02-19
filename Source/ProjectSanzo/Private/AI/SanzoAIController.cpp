#include "AI/SanzoAIController.h"
#include "AI/SanzoEnemyBase.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Common/SanzoLog.h"

#pragma region Perception
ASanzoAIController::ASanzoAIController()
{
  AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
  SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
  HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));

  // 시각(Sight) 세팅
  // 감지 반경
  SightConfig->SightRadius = 1500.f;
  // 시야에서 사라지는 반경
  SightConfig->LoseSightRadius = 2000.f;
  // 시야각
  SightConfig->PeripheralVisionAngleDegrees = 90.0f;
  // 감지 대상 설정 (적, 중립, 아군 모두 감지)
  SightConfig->DetectionByAffiliation.bDetectEnemies = true;
  SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
  SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

  // 청각(Hearing) 세팅
  // 감지 반경
  HearingConfig->HearingRange = 3000.f;
  // 감지 대상 설정 (적, 중립, 아군 모두 감지)
  HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
  HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
  HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;

  AIPerception->ConfigureSense(*SightConfig);
  AIPerception->ConfigureSense(*HearingConfig);
  AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());
}

void ASanzoAIController::OnPossess(APawn* InPawn)
{
  Super::OnPossess(InPawn);

  if (ASanzoEnemyBase* Enemy = Cast<ASanzoEnemyBase>(InPawn))
  {
    if (Enemy->EnemyBehaviorTree)
    {
      RunBehaviorTree(Enemy->EnemyBehaviorTree);
    }
  }

  AIPerception->
    OnTargetPerceptionUpdated.AddDynamic(this, &ASanzoAIController::OnTargetDetected);
}

// 감지 이벤트 처리 함수
void ASanzoAIController::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{
  if (Actor && Actor->ActorHasTag("Player"))
  {
    if (Stimulus.WasSuccessfullySensed())
    {
      // 플레이어 발견 -> 블랙보드에 타겟 저장
      GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), Actor);

      if (Stimulus.Type == SightConfig->GetSenseID())
      {
        UE_LOG(LogKDJ, Warning, TEXT("Player Detected by Sight!"));
      }
      else if (Stimulus.Type == HearingConfig->GetSenseID())
      {
        UE_LOG(LogKDJ, Warning, TEXT("Player Detected by Hearing!"));
      }
    }
    else
    {
      // 놓침
      UE_LOG(LogKDJ, Log, TEXT("Player Lost!"));
    }
  }
}
#pragma endregion 김동주