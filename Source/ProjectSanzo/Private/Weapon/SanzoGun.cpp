#include "Weapon/SanzoGun.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Components/ArrowComponent.h"
#include "TimerManager.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"

ASanzoGun::ASanzoGun()
{
	BaseDamage = 20.0f;       // 기본 데미지
	FireRate = 0.1f;          // 0.1초마다 발사 (빠른 연사)
	MaxRange = 5000.0f;       // 사거리 50미터           
	CurrentAmmo = 30;         // 시작 탄약
	bInfiniteAmmo = false;
	TracerTargetName = TEXT("Target");
}

void ASanzoGun::StartFire()
{
	Super::StartFire();

	if (!bInfiniteAmmo && CurrentAmmo <= 0) return;

	Fire();

	GetWorld()->GetTimerManager().SetTimer(
		FireTimerHandle,
		this,
		&ASanzoGun::Fire,
		FireRate,
		true
	);

}

void ASanzoGun::StopFire()
{
	Super::StopFire();

	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);

}

void ASanzoGun::Fire()
{

	if (!bInfiniteAmmo && CurrentAmmo <= 0)
	{
		StopFire();
		return;
	}

	CurrentAmmo--;
	PlayFireEffects();

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;
	AController* OwnerController = OwnerPawn->GetController();
	if (!OwnerController) return;

	// 카메라에서 정보값 가져오기
	FVector CameraLocation;
	FRotator CameraRotation;
	OwnerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector Start = CameraLocation;
	FVector End = Start + (CameraRotation.Vector() * MaxRange);
	
	// 라인트레이스 시작될 때 쏜 무기와 무기 들고있는 플레이어 판정 제외
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);           
	QueryParams.AddIgnoredActor(GetOwner());

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility, 
		QueryParams
	);

	if (TracerEffect && FireStartLocation)
	{
		FVector MuzzleLocation = FireStartLocation->GetComponentLocation();

		// 트레이스 목표 지점 설정
		FVector TraceEndPoint = bHit ? HitResult.ImpactPoint : End;

		// 총구에서 목표 지점으로 가기 위한 회전값 계산
		FRotator LaunchRotation = (TraceEndPoint - MuzzleLocation).Rotation();

		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			TracerEffect,
			MuzzleLocation,
			LaunchRotation,
			true
		);
	}
	
	// 데미지 적용
	if (bHit)
	{
		ApplyDamageToTarget(HitResult.GetActor(), HitResult);
	}
}

void ASanzoGun::AddAmmo()
{
	CurrentAmmo++;
	UE_LOG(LogTemp, Log, TEXT("Current Ammo: %d"), CurrentAmmo);
}
