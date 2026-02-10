#include "Core/SanzoGameState.h"
#include "Core/SanzoGameInstance.h"
#include "Kismet/GameplayStatics.h"

ASanzoGameState::ASanzoGameState()
{
  CurrentStageIndex = 0;
}
void ASanzoGameState::BeginPlay()
{
  Super::BeginPlay();

  if (UGameInstance* GI = GetGameInstance())
  {
    USanzoGameInstance* SGI = Cast<USanzoGameInstance>(GI);
    if (SGI)
    {
      //CurrentStageIndex = SGI->CurrentLevelIndex;
    }
  }
}