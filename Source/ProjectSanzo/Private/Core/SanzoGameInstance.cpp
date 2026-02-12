#include "Core/SanzoGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Common/SanzoLog.h"

USanzoGameInstance::USanzoGameInstance()
{
  CurrentStageIndex = 0;

}

#pragma region 스테이지 이동
void USanzoGameInstance::MoveToNextStage()
{
  // 스테이지 이동 전 정보 업데이트(save)
  UE_LOG(LogCYS, Warning, TEXT("정보 업데이트 함수 호출"), CurrentStageIndex);

  // 다음 스테이지로 이동
  if (CurrentStageIndex >= StageLevels.Num() - 1)
  {
    UE_LOG(LogCYS, Warning, TEXT("GI: 이동할 스테이지가 없음"));
    return;
  }

  CurrentStageIndex++;
  UE_LOG(LogCYS, Warning, TEXT("GI: 다음 스테이지로 이동, 인덱스: %d"), CurrentStageIndex);

  UGameplayStatics::OpenLevel(
    this,
    FName(*StageLevels[CurrentStageIndex].GetAssetName())
  );
}
#pragma endregion 최윤서

#pragma region 정보 저장 및 관리
#pragma endregion 최윤서
