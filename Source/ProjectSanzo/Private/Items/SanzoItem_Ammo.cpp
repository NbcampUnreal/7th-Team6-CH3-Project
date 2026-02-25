#include "Items/SanzoItem_Ammo.h"

#pragma region ItemAmmo
ASanzoItem_Ammo::ASanzoItem_Ammo()
{
  AmmoAmount = FMath::RandRange(10, 30);
  ItemType = "Ammo";
}

void ASanzoItem_Ammo::ActivateItem(AActor* Activator)
{
  Super::ActivateItem(Activator);

  if (Activator && Activator->ActorHasTag("Player"))
  {
    // TODO: 탄약 증가 로직
    DestroyItem();
  }
}
#pragma endregion 최윤서