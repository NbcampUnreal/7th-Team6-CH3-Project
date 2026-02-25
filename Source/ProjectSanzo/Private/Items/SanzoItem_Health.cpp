#include "Items/SanzoItem_Health.h"
#include "Common/SanzoLog.h"
#include "Character/Components/SanzoStatComponent.h"

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
    if (USanzoStatComponent* StatComp = Activator->FindComponentByClass<USanzoStatComponent>())
    {
      StatComp->RestoreHealth(HealAmount);
    }
    // TODO: 체력 증가 로직
    UE_LOG(LogCYS, Warning, TEXT("HP 획득 : %d"), HealAmount);
    DestroyItem();
  }
}
#pragma endregion 최윤서