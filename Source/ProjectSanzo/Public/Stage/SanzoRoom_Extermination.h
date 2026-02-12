#pragma once

#include "CoreMinimal.h"
#include "Stage/SanzoRoomBase.h"
#include "SanzoRoom_Extermination.generated.h"

UCLASS()
class PROJECTSANZO_API ASanzoRoom_Extermination : public ASanzoRoomBase
{
  GENERATED_BODY()
#pragma region Extermination Room
public:
  virtual void BeginRoomSequence() override;
  virtual void EndRoomSequence() override;

  // 적 처치
  int32 TotalEnemyCount;
  int32 CurrentEnemyCount;
  virtual void OnEnemyKilled() override;
#pragma endregion 최윤서
};
