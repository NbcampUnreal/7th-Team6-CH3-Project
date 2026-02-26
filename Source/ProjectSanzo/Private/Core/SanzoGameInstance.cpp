#include "Core/SanzoGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Common/SanzoLog.h"
#include "Character/SanzoCharacter.h"
#include "Weapon/SanzoWeaponBase.h"
#include "Character/Components/SanzoEquipmentComponent.h"

USanzoGameInstance::USanzoGameInstance()
{
  InitSetup();
}

#pragma region Stage Movement
void USanzoGameInstance::MoveToNextStage()
{
  // 스테이지 이동 전 정보 업데이트(save)
  UE_LOG(LogCYS, Warning, TEXT("정보 업데이트 함수 호출"), CurrentStageIndex);
  BackupStat(Cast<ASanzoCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)));
  // 다음 스테이지로 이동
  CurrentStageIndex++;
  if (CurrentStageIndex >= StageLevels.Num())
  {
    UE_LOG(LogCYS, Warning, TEXT("GI: 처음으로 돌아감"));
    InitSetup();
  }

  UE_LOG(LogCYS, Warning, TEXT("GI: 다음 스테이지로 이동, 인덱스: %d"), CurrentStageIndex);

  UGameplayStatics::OpenLevel(
    this,
    FName(*StageLevels[CurrentStageIndex].GetAssetName())
  );
}
#pragma endregion 최윤서

#pragma region Save & Load
void USanzoGameInstance::BackupStat(ASanzoCharacter* Player)
{
  if (USanzoStatComponent* StatComp = Player->FindComponentByClass<USanzoStatComponent>())
  {
    UE_LOG(LogCYS, Warning, TEXT("GI: 스탯 백업"));
    CachedStatData = StatComp->GetSaveData();
  }
  if (USanzoEquipmentComponent* EquipComp = Player->FindComponentByClass<USanzoEquipmentComponent>())
  {
    if (EquipComp->Inventory.IsValidIndex(0))
    {
      ASanzoWeaponBase* Gun = EquipComp->Inventory[0];
      CachedAmmo = Gun->GetCurrentAmmo();
      UE_LOG(LogCYS, Warning, TEXT("GI: 탄약 백업, 탄약: %d"),CachedAmmo);

    }
  }
}
void USanzoGameInstance::RestoreStat(ASanzoCharacter* Player)
{
  if (USanzoStatComponent* StatComp = Player->FindComponentByClass<USanzoStatComponent>())
  {
    UE_LOG(LogCYS, Warning, TEXT("GI: 스탯 복원, 레벨: %d"), CachedStatData.Level);
    StatComp->LoadFromSaveData(CachedStatData);
  }
  if (USanzoEquipmentComponent* EquipComp = Player->FindComponentByClass<USanzoEquipmentComponent>())
  {
    if (EquipComp->Inventory.IsValidIndex(0))
    {
      ASanzoWeaponBase* Gun = EquipComp->Inventory[0];
      Gun->SetCurrentAmmo(CachedAmmo);
      UE_LOG(LogCYS, Warning, TEXT("GI: 복원 후 탄약: %d"), Gun->GetCurrentAmmo());
    }
  }
}

void USanzoGameInstance::InitSetup()
{
  UE_LOG(LogCYS, Warning, TEXT("GI: Init Set up"));
  CurrentStageIndex = 0;
  CachedAmmo = 1000; // 초기 탄약값 설정
  CachedStatData = FSanzoSaveStatData(); // 초기 스탯값 설정
}
#pragma endregion 최윤서
