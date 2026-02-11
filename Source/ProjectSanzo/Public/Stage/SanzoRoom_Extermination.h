#pragma once

#include "CoreMinimal.h"
#include "Stage/SanzoRoomBase.h"
#include "SanzoRoom_Extermination.generated.h"

UCLASS()
class PROJECTSANZO_API ASanzoRoom_Extermination : public ASanzoRoomBase
{
  GENERATED_BODY()

public:
  virtual void BeginRoomSequence() override;
  virtual void EndRoomSequence() override;
};
