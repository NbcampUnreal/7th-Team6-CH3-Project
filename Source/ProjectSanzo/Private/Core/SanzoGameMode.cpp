#include "Core/SanzoGameMode.h"
#include "Core/SanzoGameState.h"
#include "Core/SanzoStageTypes.h"
#include "Core/SanzoGameInstance.h" 
#include "Character/SanzoCharacter.h"
#include "Character/SanzoPlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "Stage/SanzoStageManager.h"
#include "EngineUtils.h"

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
  if(CurrentStageType!=ESanzoStageType::None)
  {
    StartStage();
  }
}

// 맵 이름에 따라 스테이지 타입 설정
void ASanzoGameMode::InitStageType()
{
  if (GetWorld()->GetMapName().Contains("Stage1"))
  {
    UE_LOG(LogTemp, Warning, TEXT("GM: 스테이지 타입 설정 - 섬멸전"));
    CurrentStageType = ESanzoStageType::Extermination;
  }
  else if (GetWorld()->GetMapName().Contains("Stage2"))
  {
    UE_LOG(LogTemp, Warning, TEXT("GM: 스테이지 타입 설정 - 버티기"));
    CurrentStageType = ESanzoStageType::Survival;
  }
  else if (GetWorld()->GetMapName().Contains("Stage3"))
  {
    UE_LOG(LogTemp, Warning, TEXT("GM: 스테이지 타입 설정 - 보스전"));
    CurrentStageType = ESanzoStageType::Boss;
  }
  else
  {
    UE_LOG(LogTemp, Warning, TEXT("GM: 스테이지 타입 없음"));
    CurrentStageType = ESanzoStageType::None;
  }
}
void ASanzoGameMode::StartStage()
{
  // StageManager에게 스테이지 시작 지시
  UE_LOG(LogTemp, Warning, TEXT("GM: 스테이지 시작"));
  ASanzoStageManager* SM = nullptr;
  for (TActorIterator<ASanzoStageManager> It(GetWorld()); It; ++It)
  {
    SM = *It;
    break;
  }
  if (SM)
  {
    SM->StartStage();
  }
}
void ASanzoGameMode::OnStageCleared()
{
  UE_LOG(LogTemp, Warning, TEXT("GM: 스테이지 클리어"));
  
  // 스테이지 이동
  USanzoGameInstance* SGI = Cast<USanzoGameInstance>(GetGameInstance());
  if (SGI)
  {
    SGI->MoveToNextStage();
  }
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
