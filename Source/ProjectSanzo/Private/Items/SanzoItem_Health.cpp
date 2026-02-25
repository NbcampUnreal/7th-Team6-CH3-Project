#include "Items/SanzoItem_Health.h"

#pragma region ItemHealth
ASanzoItem_Health::ASanzoItem_Health()
{
  HealAmount = 20;
  ItemType = "Health";
}

void ASanzoItem_Health::ActivateItem(AActor* Activator)
{
  Super::ActivateItem(Activator);

  if (Activator && Activator->ActorHasTag("Player"))
  {
    // TODO: 체력 증가 로직
    DestroyItem();
  }
}
#pragma endregion 최윤서