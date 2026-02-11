#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SanzoWeaponBase.generated.h"

UCLASS()
class PROJECTSANZO_API ASanzoWeaponBase : public AActor
{
  GENERATED_BODY()

public:
  ASanzoWeaponBase();

protected:

	// 무기 애니메이션이 있을 수 있어서 SkeletalMesh 선택
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Components")
	USkeletalMeshComponent* WeaponMesh;
	// 투사체 발사 시작 위치, 히트스캔 시작 위치
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Components")
	class UArrowComponent* FireStartLocation;
	// 최대 탄약 제한 (필요 없으면 삭제)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Ammo")
	int32 MaxAmmo;
	// 무기 중 총 사용 시 필요(UI에서도 필요할수도)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Ammo")
	int32 CurrentAmmo;
	// 플레이어 활, AI 적 원거리 무기 = true, 플레이어 총 = false
	bool bInfiniteAmmo;
	// 기본 사거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Stats")
	float MaxRange;
	// 기본 공격력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Stats")
	float BaseDamage;
	// 연사 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Stats")
	float FireRate;
	// 캐릭터가 재생할 애니메이션
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UAnimMontage* CharacterFireMontage;
	// 총기에서 발생할 애니메이션
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UAnimationAsset* WeaponFireAnim;

public:
	// 마우스 좌클릭 눌렀을 때 작동, 활이면 차징 시작, 총(연사)면 발사~, 총(단발)이면 Fire() 1번 호출 
	UFUNCTION(BlueprintCallable, Category = "WeaponAction")
	virtual void StartFire();
	// 마우스 좌클릭 땠을 때 작동, 활이면 발사, 총(연사)면  발사 중지
	UFUNCTION(BlueprintCallable, Category = "WeaponAction")
	virtual void StopFire();
	// 재장전 애니메이션 실행용

protected:
	// 실제 발사 로직, 자녀 클래스(bow, gun 등) 에서 오버라이딩 필요
	virtual void Fire() {};
	// 사운드, 이펙트 담장 로직, 부모 클래스에서 로직 담당, 자식 클래스에서 사운드, 이펙트 종류 정함
	virtual void PlayFireEffects();
	// 데미지 전달용(함수 내부에서 플레이어나 적의 상태(버프, 강화 등) 에 따라서 최종데미지 계산)
	void ApplyDamageToTarget(AActor * TargetActor, FHitResult HitInfo);
	// 플레이어 총기 사용 시 연사 발사 속도 제어용 타이머
	FTimerHandle FireTimerHandle;

};
