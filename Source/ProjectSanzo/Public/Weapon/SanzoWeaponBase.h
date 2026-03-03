#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/UpgradeSystem/UpgradeDataRow.h"
#include "SanzoWeaponBase.generated.h"

#pragma region DataForHUD
	
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyHit);

#pragma endregion 이준로

class UNiagaraSystem;

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

	// 무기 중 총 사용 시 필요(UI에서도 필요할수도)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Ammo")
	int32 CurrentAmmo;

	// 플레이어 활, AI 적 원거리 무기 = true, 플레이어 총 = false
	bool bInfiniteAmmo;

	// 공격력 원본 값
	float OriginalBaseDamage;

	// 현재 공격력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Stats")
	float BaseDamage;

	// 헤드샷 데미지 배율
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Stats")
	float HeadshotMultiplier = 2.0f;

	// 헤드샷 판정할 뼈 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Stats")
	FName HeadBoneName = FName("head");

	// 무기를 들고 있는 캐릭터가 재생할 애니메이션
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Effects")
	UAnimMontage* CharacterFireMontage;

	// 무기에서 발생할 애니메이션
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Effects")
	UAnimationAsset* WeaponFireAnim;

	// 적말고 다른 곳에 맞았을 때 효과
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Effects")
	UParticleSystem* ImpactEffect;

	// 적이 맞았을 때 피 튀는 효과
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Effects")
	UNiagaraSystem* BloodSplatterEffect;

	// 벽에 남을 총알 자국 (데칼)
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Effects")
	UMaterialInterface* BulletHoleDecal;

	// 총알 자국 사이즈 조절용 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Effects")
	float DecalSize = 8.0f;

#pragma region HitReact
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Effects")
	USoundBase* HitSuccessSound;
#pragma endregion 김형백

public:
	
	virtual void BeginPlay() override;

	// 마우스 좌클릭 눌렀을 때 작동, 활이면 차징 시작, 총(연사)면 발사~, 총(단발)이면 Fire() 1번 호출 
	UFUNCTION(BlueprintCallable, Category = "WeaponAction")
	virtual void StartFire();

	// 마우스 좌클릭 땠을 때 작동, 활이면 발사, 총(연사)면  발사 중지
	UFUNCTION(BlueprintCallable, Category = "WeaponAction")
	virtual void StopFire();

	// 데미지 전달용(함수 내부에서 플레이어나 적의 상태(버프, 강화 등) 에 따라서 최종데미지 계산)
	void ApplyDamageToTarget(AActor* TargetActor, FHitResult HitInfo, float DamageToApply);

	// 맞은 대상의 이펙트 출력 담당
	virtual void PlayImpactEffects(FHitResult HitInfo);

	// UI에서 사용할 현재 총알 값 가져오는 함수
	UFUNCTION(BlueprintPure, Category = "Weapon|Ammo")
	int32 GetCurrentAmmo() const { return CurrentAmmo; }

	// 총알 Setter - 최윤서
	UFUNCTION(Category = "Weapon|Ammo")
	virtual void SetCurrentAmmo(int32 Ammo) { CurrentAmmo = Ammo; }

	//업그레이드 적용 함수
	virtual void ApplyWeaponStatUpgrade(EUpgradeType Type, float Value);
	// 무기 장착했을 때 캐릭터 메쉬에 적용할 ABP 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Swap")
	TSubclassOf<class UAnimInstance> WeaponAnimInstanceClass;

	// 무기가 부착될 소켓 이름
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon|Attachment")
	FName AttachSocketName;

	// 스왑할 때 플레이할 애니메이션 시퀀스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Animation")
	class UAnimMontage* EquipMontage;
  
	//다른곳에서 매시 부르기 -김형백
	USkeletalMeshComponent* GetWeaponMesh() { return WeaponMesh; }

protected:
	// 실제 발사 로직, 자녀 클래스(bow, gun 등) 에서 오버라이딩 필요
	virtual void Fire() {};

	// 총에서 나갈 이펙트, 사운드 출력 담당, 에디터에서 수정 가능
	virtual void PlayFireEffects();

	// 플레이어 총기 사용 시 연사 발사 속도 제어용 타이머
	FTimerHandle FireTimerHandle;
	
#pragma region DataForHUD
	
public:
	FOnEnemyHit OnEnemyHit;
	
	virtual FText GetAmmoTextForHUD() const { return FText::GetEmpty();}
	
	float GetBaseDamage() const { return BaseDamage; }
	void SetBaseDamage(float NewDamage) { BaseDamage = NewDamage; }
	
#pragma endregion 이준로

	//김형백 화살몽타주시작 델리게이트
	FOnMontageBlendedInEnded StartBowAttackDelegate;
};

