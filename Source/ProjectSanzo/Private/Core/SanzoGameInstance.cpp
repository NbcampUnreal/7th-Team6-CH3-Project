#include "Core/SanzoGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Common/SanzoLog.h"

USanzoGameInstance::USanzoGameInstance()
{
  CurrentStageIndex = 0;

}

#pragma region Stage Movement
void USanzoGameInstance::MoveToNextStage()
{
  // 스테이지 이동 전 정보 업데이트(save)
  UE_LOG(LogCYS, Warning, TEXT("정보 업데이트 함수 호출"), CurrentStageIndex);

  // 다음 스테이지로 이동
  CurrentStageIndex++;
  if (CurrentStageIndex >= StageLevels.Num())
  {
    UE_LOG(LogCYS, Warning, TEXT("GI: 처음으로 돌아감"));
    CurrentStageIndex = 0;
  }

  UE_LOG(LogCYS, Warning, TEXT("GI: 다음 스테이지로 이동, 인덱스: %d"), CurrentStageIndex);

  UGameplayStatics::OpenLevel(
    this,
    FName(*StageLevels[CurrentStageIndex].GetAssetName())
  );
}
#pragma endregion 최윤서

#pragma region Save & Load
#pragma endregion 최윤서
