#pragma once

#include "CoreMinimal.h"
#include "SanzoStageTypes.generated.h"

UENUM(BlueprintType)
enum class ESanzoStageType : uint8
{
  Extermination UMETA(DisplayName = "Extermination"),
  Survival      UMETA(DisplayName = "Survival"),
  Boss          UMETA(DisplayName = "Boss")
};
