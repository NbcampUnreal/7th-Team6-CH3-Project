#pragma once

#include "CoreMinimal.h"
#include "Weapon/SanzoWeaponBase.h"
#include "SanzoBow.generated.h"

class ASanzoProjectile;

UCLASS()
class PROJECTSANZO_API ASanzoBow : public ASanzoWeaponBase
{
  GENERATED_BODY()

public:
	ASanzoBow();

	virtual void StartFire() override;

	virtual void StopFire() override;

	virtual void Fire() override;

	virtual void ApplyWeaponStatUpgrade(EUpgradeType Type, float Value) override;

protected:

	// 발사할 때마다 생성할 투사체 설계도
	UPROPERTY(EditDefaultsOnly, Category = "Bow|Projectile")
	TSubclassOf<ASanzoProjectile> ProjectileClass;

	// 좌클릭 눌렀을 때 시간 넣기용
	float ChargeStartTime;

	// 최대 차징 유효 시간(현재는 3초, 수정가능)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow|Charge")
	float MaxChargeTime = 2.0f;

	// 계산 후 차징퍼센티지 넣을 변수
	float ChargePercent;

	// 클릭만 했을 때 넣을 최소속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow|Charge")
	float MinArrowSpeed = 500.0f;

	// 풀차징했을 때 최대속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow|Charge")
	float MaxArrowSpeed = 6000.0f;

	// 차징에 따른 최소 데미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow|Charge")
	float MinArrowDamage = 5.0f;

	// 차징에 따른 최대 데미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow|Charge")
	float MaxArrowDamage = 30.0f;
	
#pragma region DataForHUD
	
public:
	virtual FText GetAmmoTextForHUD() const override;
	
#pragma endregion 이준로
};
