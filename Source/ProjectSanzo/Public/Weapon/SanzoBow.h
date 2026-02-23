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

protected:

	// 발사할 때마다 생성할 투사체 설계도
	UPROPERTY(EditDefaultsOnly, Category = "Bow|Projectile")
	TSubclassOf<ASanzoProjectile> ProjectileClass;
};
