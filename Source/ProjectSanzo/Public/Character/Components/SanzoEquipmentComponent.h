// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Character/Interface/SanzoUpgradeInterface.h"
#include "SanzoEquipmentComponent.generated.h"

#pragma region UIDataTransfer

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponAmmoChanged, FText, NewAmmo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponSwapped, int32, CurrentIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAnyWeaponHitEnemy);

#pragma endregion 이준로

class ASanzoCharacter;

enum class WeaponName : uint8
{
  Gun,
  Bow
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTSANZO_API USanzoEquipmentComponent : public UActorComponent, public ISanzoUpgradeInterface
{
  GENERATED_BODY()

public:
  USanzoEquipmentComponent();

protected:
  virtual void BeginPlay() override;
  ASanzoCharacter* GetOwnerCharacter();

  virtual void ApplyUpgrade(EUpgradeTarget Target, EUpgradeType Type, float Value) override;

public:
  virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
#pragma region 스왑 시스템
  // 무기 교체 함수
  UFUNCTION(BlueprintCallable, Category = "Equipment|Action")
  void SwapWeapon(bool bUpdateAnimInstance = true);

  // 내부 인덱스에 맞는 무기를 장착시키는 함수
  void EquipWeaponByIndex(int32 Index, bool bUpdateAnimInstance = true);

  // 캐릭터가 호출할 스왑 시작 함수
  UAnimMontage* BeginSwapWeapon();

  // 원래 무기 하나 소환했지만 무기를 인덱스로 갖는 배열로 수정
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment|Class")
  TArray<TSubclassOf<class ASanzoWeaponBase>> DefaultWeaponClasses;

  // (이용호 추가)실제 월드에 스폰되어 인벤토리에 들어온 무기 인스턴스
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment|Instances")
  TArray<ASanzoWeaponBase*> Inventory;
#pragma endregion 이용호
  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Equipment|Class")
  ASanzoWeaponBase* CurrentWeapon;
  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Equipment|Class")
  ASanzoCharacter* OwnerCharacter;

  // (이용호 추가) 현재 들고 있는 무기의 인덱스 저장용 변수
  int32 CurrentWeaponIndex = 0;

  void AddAmmo(int32 Amount);

#pragma region UIDataTransfer

public:
	UFUNCTION()
	ASanzoWeaponBase* GetCurrentWeapon() const { return CurrentWeapon; }
	
  FOnWeaponAmmoChanged OnAmmoChanged;
	FOnWeaponSwapped OnSwapped;
	FOnAnyWeaponHitEnemy OnAnyWeaponHitEnemy;

protected:
  UFUNCTION()
  void UpdateHUDAmmo();
	UFUNCTION()
	void HandleWeaponHitEnemy();

#pragma endregion 이준로
};
