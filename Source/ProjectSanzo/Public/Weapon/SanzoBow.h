#pragma once

#include "CoreMinimal.h"
#include "Weapon/SanzoWeaponBase.h"
#include "SanzoBow.generated.h"

class ASanzoProjectile;

#pragma region DataForHUD

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChargePercentChanged, float, NewPercnetage);

#pragma endregion 이준로

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

	// 계산 후 차징퍼센티지 넣을 변수
	float ChargePercent;

	// 클릭만 했을 때 넣을 최소속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow|Charge")
	float MinArrowSpeed = 500.0f;

	// 풀차징했을 때 최대속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow|Charge")
	float MaxArrowSpeed = 6000.0f;

	// 활시위가 당겨지는 애니메이션 시퀀스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow|Animation")
	class UAnimSequence* BowDrawAnim;

	// 시위 당길 때 보일 가짜 화살 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bow|Components")
	class UStaticMeshComponent* DummyArrowMesh;

	// 가짜 화살이 부착될 활 시위 쪽의 소켓
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Bow|Attachment")
	FName StringSocketName;

	// 활 발사 시작했을 때 소리 넣을 사운드베이스
	UPROPERTY(EditDefaultsOnly, Category = "Bow|Sound")
	USoundBase* DrawStartSound;
	// 활 차징 계속 당기고 있을 때 소리 넣을 사운드베이스
	UPROPERTY(EditDefaultsOnly, Category = "Bow|Sound")
	USoundBase* DrawLoopSound;
	// 활 발사했을 때 소리 넣을 사운드베이스
	UPROPERTY(EditDefaultsOnly, Category = "Bow|Sound")
	USoundBase* FireSound;

	// 사운드들 제어할 오디오 컴포넌트
	UPROPERTY()
	UAudioComponent* DrawStartAudioComp;

	UPROPERTY()
	UAudioComponent* DrawLoopAudioComp;

	UPROPERTY()
	UAudioComponent* FireAudioComp;

	// 차징 완료 시간에 맞춰 소리를 넘겨줄 타이머
	FTimerHandle SwitchSoundTimer;

	// 타이머가 실행할 함수
	void SwitchToLoopSound();

	// 멀티샷이 허용되었는지 확인해 줄 변수
	UPROPERTY(EditAnywhere, Category = "Bow|Upgrade")
	bool bIsMultiShotEnabled = false;

#pragma region DataForHUD
	
public:
	
	virtual void Tick(float DeltaSeconds) override;
	
	UPROPERTY()
	FOnChargePercentChanged OnChargePercentChanged;
	
	virtual FText GetAmmoTextForHUD() const override;

	// 최대 차징 유효 시간(현재는 3초, 수정가능)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow|Charge")
	float MaxChargeTime = 1.0f;
	
	float GetMaxChargeTime() const { return MaxChargeTime; }
	
protected:
	bool bIsCharging = false;
	
#pragma endregion 이준로
};
