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
  SightConfig->SightRadius = 3500.f;
  // 시야에서 사라지는 반경
  SightConfig->LoseSightRadius = 3000.f;
  // 시야각
  SightConfig->PeripheralVisionAngleDegrees = 90.0f;
  // 감지 대상 설정 (적, 중립, 아군 모두 감지)
  SightConfig->DetectionByAffiliation.bDetectEnemies = true;
  SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
  SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

  // 청각(Hearing) 세팅
  // 감지 반경
  HearingConfig->HearingRange = 5000.f;
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
    UBlackboardComponent* BBComp = GetBlackboardComponent();
    if (!BBComp) return;

    if (Stimulus.WasSuccessfullySensed())
    {
      // [시각] 직접 플레이어를 봤을 때
      if (Stimulus.Type == SightConfig->GetSenseID())
      {
        BBComp->SetValueAsObject(TEXT("TargetActor"), Actor);
        BBComp->ClearValue(TEXT("InvestigateLocation"));
        UE_LOG(LogKDJ, Warning, TEXT("Player Detected by Sight!"));
      }
      // [청각] 총소리(플레이어의 공격)를 들었을 때
      else if (Stimulus.Type == HearingConfig->GetSenseID())
      {
        if (BBComp->GetValueAsObject(TEXT("TargetActor")) == nullptr)
        {
          BBComp->SetValueAsVector(TEXT("InvestigateLocation"), Stimulus.ReceiverLocation);
        }
        UE_LOG(LogKDJ, Warning, TEXT("Player Detected by Hearing!"));
      }
    }
    else
    {
      // 플레이어가 시야 범위 밖으로 나가거나 벽에 숨어 감지가 끊기면
      if (Stimulus.Type == SightConfig->GetSenseID())
      {
        // 타겟을 지워서 공격 상태 해제
        BBComp->ClearValue(TEXT("TargetActor"));

        // 플레이어의 마지막 위치를 기억
        BBComp->SetValueAsVector(TEXT("InvestigateLocation"), Actor->GetActorLocation());

        UE_LOG(LogKDJ, Log, TEXT("Player Lost! Going to Last Known Location."));
      }
    }
  }
}
#pragma endregion 김동주