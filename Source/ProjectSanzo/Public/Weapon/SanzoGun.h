#pragma once

#include "CoreMinimal.h"
#include "Weapon/SanzoWeaponBase.h"
#include "SanzoGun.generated.h"

#pragma region DataForHUD
	
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAmmoChanged);
	
#pragma endregion 이준로
class ASanzoEnemyBase;

UCLASS()
class PROJECTSANZO_API ASanzoGun : public ASanzoWeaponBase
{
  GENERATED_BODY()

public:
	ASanzoGun();

	// 마우스 누름: 연사 타이머 시작
	virtual void StartFire() override;

	// 마우스 뗌: 연사 타이머 중지
	virtual void StopFire() override;

protected:

	// 에셋에 있는 트레이스 이펙트 사용위한 변수
	UPROPERTY(EditDefaultsOnly, Category = "Gun|Effects")
	UParticleSystem* TracerEffect;

	// 디버그용 라인 트레이스
	UPROPERTY(EditAnywhere, Category = "Gun|Debug")
	bool bShowDebugTrace = false;

	// 마지막으로 총을 쏜 시간을 기록할 변수(단발 광클 방지용)
	float LastFireTime;

	// 실제 발사 로직: 라인트레이스 및 디버그 라인 그리기
	virtual void Fire() override;

	// 호밍 미사일 발사 확률
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun|Upgrade")
	float HomingMissileChance;

	// 호밍 미사일 설계도 넣을 변수
	UPROPERTY(EditDefaultsOnly, Category = "Gun|Projectile")
	TSubclassOf<class ASanzoProjectile> HomingProjectileClass;

	// 화면에 보이는 적들 중 랜덤으로 하나 뽑는 함수
	ASanzoEnemyBase* FindRandomVisibleEnemy();

	// 미사일 스폰하고 방향 정할 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gun|Components")
	class UArrowComponent* MissileSpawnLocation;

	// 총 연사 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun|Stats")
	float FireRate;

public:

	virtual void ApplyWeaponStatUpgrade(EUpgradeType Type, float Value) override;

	// 후에 드랍된 총알 먹었을 때 탄약 보충될 함수
	void AddAmmo(int32 Amount);
	
  float GetFireRate() const { return FireRate; }
	void SetFireRate(float NewFireRate) { FireRate = NewFireRate; }
	float GetHomingMissileChance() const { return HomingMissileChance; }
	void SetHomingMissileChance(float Chance) { HomingMissileChance = Chance; }
#pragma region DataForHUD
	
public:
	
	void SetCurrentAmmo(int32 Amount);
	
	UPROPERTY()
	FOnAmmoChanged OnAmmoChanged;
	
	virtual FText GetAmmoTextForHUD() const override;
	
#pragma endregion 이준로
};
