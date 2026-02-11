#include "Weapon/SanzoGun.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Components/ArrowComponent.h"
#include "TimerManager.h"

ASanzoGun::ASanzoGun()
{
	BaseDamage = 20.0f;       // 기본 데미지
	FireRate = 0.1f;          // 0.1초마다 발사 (빠른 연사)
	MaxRange = 5000.0f;       // 사거리 50미터
	MaxAmmo = 30;             // 최대 탄약
	CurrentAmmo = 30;         // 시작 탄약
	bInfiniteAmmo = false;
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

	FVector CameraLocation;
	FRotator CameraRotation;
	OwnerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector Start = CameraLocation;
	FVector End = Start + (CameraRotation.Vector() * MaxRange);

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

	if (bHit)
	{
		// 어딘가 맞았으면: 총구 -> 맞은 곳까지만 그림
		DrawDebugLine(GetWorld(), FireStartLocation->GetComponentLocation(), HitResult.ImpactPoint, FColor::Red, false, 5.0f, 0, 1.0f);

		// 데미지 전달
		ApplyDamageToTarget(HitResult.GetActor(), HitResult);
	}
	else
	{
		// 안 맞았으면(허공): 총구 -> 사거리 끝까지 그림
		DrawDebugLine(GetWorld(), FireStartLocation->GetComponentLocation(), End, FColor::Red, false, 5.0f, 0, 1.0f);
	}
}

void ASanzoGun::AddAmmo(int32 Amount)
{
	CurrentAmmo = FMath::Clamp(CurrentAmmo + Amount, 0, MaxAmmo);
	UE_LOG(LogTemp, Log, TEXT("Ammo Added: %d / Current: %d"), Amount, CurrentAmmo);
}
