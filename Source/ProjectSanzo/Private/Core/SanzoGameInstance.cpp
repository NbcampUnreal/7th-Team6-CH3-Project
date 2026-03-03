#include "Core/SanzoGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Common/SanzoLog.h"
#include "Character/SanzoCharacter.h"
#include "Weapon/SanzoWeaponBase.h"
#include "Character/Components/SanzoEquipmentComponent.h"
#include "Core/UpgradeSystem/SanzoUpgradeSubsystem.h"
#include "Weapon/SanzoGun.h"
#include "Weapon/SanzoBow.h"

USanzoGameInstance::USanzoGameInstance()
{
  InitSetup();
}

#pragma region Stage Movement
void USanzoGameInstance::MoveToNextStage()
{
  // 스테이지 이동 전 정보 업데이트(save)
  BackupStat(Cast<ASanzoCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)));
  // 다음 스테이지로 이동
  CurrentStageIndex++;
  if (CurrentStageIndex >= StageLevels.Num())
  {
    Restart();
  }

  UGameplayStatics::OpenLevel(
    this,
    FName(*StageLevels[CurrentStageIndex].GetAssetName())
  );
}
void USanzoGameInstance::Restart()
{
  InitSetup();
  UGameplayStatics::OpenLevel(
    this,
    FName(*StageLevels[CurrentStageIndex].GetAssetName())
  );
}

#pragma endregion 최윤서
#pragma region Player Stat & Upgrade Reset
void USanzoGameInstance::StartNewGame()
{
  InitSetup();
  CurrentStageIndex = 1;
  UGameplayStatics::OpenLevel(
    this,
    FName(*StageLevels[CurrentStageIndex].GetAssetName())
  );
}

#pragma endregion 이용호
#pragma region Save & Load
void USanzoGameInstance::BackupStat(ASanzoCharacter* Player)
{
  if (USanzoStatComponent* StatComp = Player->FindComponentByClass<USanzoStatComponent>())
  {
    CachedStatData = StatComp->GetSaveData();
  }
  if (USanzoEquipmentComponent* EquipComp = Player->FindComponentByClass<USanzoEquipmentComponent>())
  {
    // 인벤토리 돌면서 모든 무기 보기
    for (ASanzoWeaponBase* Weapon : EquipComp->Inventory)
    {
      // 무기가 총이라면 총 저장 변수에 현재 값 저장
      if (ASanzoGun* Gun = Cast<ASanzoGun>(Weapon))
      {
        CachedGunDamage = Gun->GetBaseDamage();
        CachedGunFireRate = Gun->GetFireRate();
        CachedAmmo = Gun->GetCurrentAmmo();
        CachedHomingMissileChance = Gun->GetHomingMissileChance();
      }
      // 무기가 활이라면 활 저장 변수에 현재 값 저장
      else if (ASanzoBow* Bow = Cast<ASanzoBow>(Weapon))
      {
        CachedBowDamage = Bow->GetBaseDamage();
        CachedBowChargeTime = Bow->GetMaxChargeTime();
        bCachedBowMultiShot = Bow->GetIsMultiShotEnabled();
      }
    }
  }
}
void USanzoGameInstance::RestoreStat(ASanzoCharacter* Player)
{
  if (USanzoStatComponent* StatComp = Player->FindComponentByClass<USanzoStatComponent>())
  {
    StatComp->LoadFromSaveData(CachedStatData);
  }
  if (USanzoEquipmentComponent* EquipComp = Player->FindComponentByClass<USanzoEquipmentComponent>())
  {
    for (ASanzoWeaponBase* Weapon : EquipComp->Inventory)
    {
      // 총 복원
      if (ASanzoGun* Gun = Cast<ASanzoGun>(Weapon))
      {
        Gun->SetCurrentAmmo(CachedAmmo);

        // 업그레이드한 적이 있을 때만 값 덮어쓰기
        if (CachedGunDamage > 0.0f)     Gun->SetBaseDamage(CachedGunDamage);
        if (CachedGunFireRate > 0.0f)   Gun->SetFireRate(CachedGunFireRate);
        if (CachedHomingMissileChance > 0.0f)   Gun->SetHomingMissileChance(CachedHomingMissileChance);
      }
      // 활 복원
      else if (ASanzoBow* Bow = Cast<ASanzoBow>(Weapon))
      {
        // 업그레이드한 적이 있을 때만 값 덮어쓰기
        if (CachedBowDamage > 0.0f)     Bow->SetBaseDamage(CachedBowDamage);
        if (CachedBowChargeTime > 0.0f) Bow->SetMaxChargeTime(CachedBowChargeTime);
        if (bCachedBowMultiShot) Bow->SetIsMultiShotEnabled(bCachedBowMultiShot);
      }
    }
  }
}

void USanzoGameInstance::InitSetup()
{
  CurrentStageIndex = 0;
  CachedAmmo = 100; // 초기 탄약값 설정
  // 무기 업그레이드 정보 백지화
  CachedGunDamage = -1.0f;
  CachedGunFireRate = -1.0f;
  CachedBowDamage = -1.0f;
  CachedBowChargeTime = -1.0f;
  CachedHomingMissileChance = -1.0f;
  bCachedBowMultiShot = false;
  CachedStatData = FSanzoSaveStatData(); // 초기 스탯값 설정

	if (USanzoUpgradeSubsystem* UpgradeSubsystem = GetSubsystem<USanzoUpgradeSubsystem>())
	{
		UpgradeSubsystem->ResetUpgradeData();
	}
	
}
#pragma endregion 최윤서
