#include "Items/SanzoItem_Ammo.h"
#include "Common/SanzoLog.h"

#include "Character/Components/SanzoEquipmentComponent.h"

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
    if (USanzoEquipmentComponent* EquipComp = Activator->FindComponentByClass<USanzoEquipmentComponent>())
    {
      EquipComp->AddAmmo(AmmoAmount);
    	//HUD에 데이터 전달
    	NotifyItemPickedUp(Activator,AmmoAmount);
    }
    
    // TODO: 탄약 증가 로직
    UE_LOG(LogCYS, Warning, TEXT("탄약 획득 : %d"), AmmoAmount);
    DestroyItem();
  }
}
#pragma endregion 최윤서