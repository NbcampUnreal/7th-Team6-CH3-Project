#include "AI/SanzoAIController.h"
#include "AI/SanzoEnemyBase.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Damage.h"
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
  HearingConfig->HearingRange = 10000.f;
  // 감지 대상 설정 (적, 중립, 아군 모두 감지)
  HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
  HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
  HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;

  // 피격 감지(Damage) 세팅
  DamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageConfig"));

  AIPerception->ConfigureSense(*SightConfig);
  AIPerception->ConfigureSense(*HearingConfig);
  AIPerception->ConfigureSense(*DamageConfig);
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

    // 최초 발견 여부 체크
    bool bIsNewDetection = (BBComp->GetValueAsObject(TEXT("TargetActor")) == nullptr);

    if (Stimulus.WasSuccessfullySensed())
    {
      // [시각] 직접 플레이어를 봤을 때
      if (Stimulus.Type == SightConfig->GetSenseID())
      {
        // 처음 발견했을 때만 느낌표 띄우기
        if (bIsNewDetection)
        {
          if (ASanzoEnemyBase* Enemy = Cast<ASanzoEnemyBase>(GetPawn()))
          {
            Enemy->ShowAlertWidget(true);
          }
        }
        BBComp->SetValueAsObject(TEXT("TargetActor"), Actor);
        SetFocus(Actor);
        BBComp->ClearValue(TEXT("InvestigateLocation"));
        UE_LOG(LogKDJ, Warning, TEXT("Player Detected by Sight!"));
      }
      // [청각] 총소리(플레이어의 공격)를 들었을 때
      else if (Stimulus.Type == HearingConfig->GetSenseID())
      {
        if (bIsNewDetection)
        {
          if (ASanzoEnemyBase* Enemy = Cast<ASanzoEnemyBase>(GetPawn()))
          {
            Enemy->ShowAlertWidget(false);
          }
          BBComp->SetValueAsVector(TEXT("InvestigateLocation"), Stimulus.StimulusLocation);
          SetFocalPoint(Stimulus.StimulusLocation);
          UE_LOG(LogKDJ, Warning, TEXT("Player Detected by Hearing!"));
        }
      }
      // [피격] 플레이어에게 공격당했을 때
      else if (Stimulus.Type == DamageConfig->GetSenseID())
      {
        // 피격 감지는 즉시 타겟 설정
        if (bIsNewDetection)
        {
          if (ASanzoEnemyBase* Enemy = Cast<ASanzoEnemyBase>(GetPawn()))
          {
            Enemy->ShowAlertWidget(true);
          }
        }
        BBComp->SetValueAsObject(TEXT("TargetActor"), Actor);
        SetFocalPoint(Stimulus.StimulusLocation);
        BBComp->ClearValue(TEXT("InvestigateLocation"));
        UE_LOG(LogKDJ, Warning, TEXT("Player Detected by DAMAGE!"));
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
        ClearFocus(EAIFocusPriority::Gameplay);

      	if (ASanzoEnemyBase* Enemy = Cast<ASanzoEnemyBase>(GetPawn()))
      	{
      		Enemy->ShowAlertWidget(false);
      	}
      	
        UE_LOG(LogKDJ, Log, TEXT("Player Lost! Going to Last Known Location."));
      }
    }
  }
}
#pragma endregion 김동주