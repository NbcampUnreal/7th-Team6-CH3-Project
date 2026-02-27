#include "Items/SanzoItem_Exp.h"
#include "Common/SanzoLog.h"
#include "Character/Components/SanzoStatComponent.h"

#pragma region ItemExp
ASanzoItem_Exp::ASanzoItem_Exp()
{
  ExpAmount = FMath::RandRange(10, 50);
  ItemType = "Exp";
}

void ASanzoItem_Exp::ActivateItem(AActor* Activator)
{
  Super::ActivateItem(Activator);
  if (Activator && Activator->ActorHasTag("Player"))
  {
    if (USanzoStatComponent* StatComp = Activator->FindComponentByClass<USanzoStatComponent>())
    {
      StatComp->AddExperience(ExpAmount);
    	//HUD에 데이터 전달
    	NotifyItemPickedUp(Activator,ExpAmount);
    }
    UE_LOG(LogCYS, Warning, TEXT("EXP 획득 : %d"), ExpAmount);
    DestroyItem();
  }
}
#pragma endregion 최윤서