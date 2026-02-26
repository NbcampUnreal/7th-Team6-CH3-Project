#include "Weapon/SanzoGun.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Components/ArrowComponent.h"
#include "TimerManager.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"

ASanzoGun::ASanzoGun()
{
	BaseDamage = 5.0f;       // 기본 데미지
	FireRate = 0.15f;          // 0.15초마다 발사 (빠른 연사)         
	CurrentAmmo = 3000;         // 시작 탄약
	bInfiniteAmmo = false;

	// 첫 발은 무조건 바로 나가도록 초기화
	LastFireTime = 0.0f;

	// 라인트레이스 시작 위치 고정
	if (FireStartLocation)
	{
		// Location (X, Y, Z)
		FireStartLocation->SetRelativeLocation(FVector(0.0f, 50.0f, 11.5f));

		// Rotation (X(Roll), Y(Pitch), Z(Yaw))
		// C++ 생성자 FRotator(Pitch, Yaw, Roll) == (Y, Z, X)
		FireStartLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));

		// Scale (X, Y, Z)
		FireStartLocation->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	}

}

void ASanzoGun::StartFire()
{
	Super::StartFire();

	// 현재 플레이 시간 저장
	float CurrentTime = GetWorld()->GetTimeSeconds();

	// 단발 광클을 해서 입력 텀이 FireRate(0.15f) 보다 작은지 계산
	float FirstDelay = FMath::Max(LastFireTime + FireRate - CurrentTime, 0.0f);

	// FirstDelay 의 값이 양수라면 그 시간만큼 기다렸다가 FireRate 시간 후에 Fire() 작동
	GetWorldTimerManager().SetTimer(
		FireTimerHandle,
		this,
		&ASanzoGun::Fire,
		FireRate,
		true,
		FirstDelay
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

	// 총을 쏜 시점을 LastFireTime 에 저장
	LastFireTime = GetWorld()->GetTimeSeconds();

	if (!bInfiniteAmmo)
	{
		CurrentAmmo--;
		//총알 개수 변화 방송
		OnAmmoChanged.Broadcast();
	}
	PlayFireEffects();

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;
	AController* OwnerController = OwnerPawn->GetController();
	if (!OwnerController) return;

	// 카메라에서 정보값 가져오기
	FVector CameraLocation;
	FRotator CameraRotation;
	OwnerController->GetPlayerViewPoint(CameraLocation, CameraRotation);
	// 사라진 사거리 대신 사용할 트레이스 끝점 
	float TraceDistance = 100000.f;

	FVector Start = CameraLocation;
	FVector End = Start + (CameraRotation.Vector() * TraceDistance);

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

	// 근접에 적 있을 때 총구 앞에 적이 있으면 판정 우선
	if (FireStartLocation)
	{
		FHitResult MuzzleHit;
		FVector MuzzleStart = FireStartLocation->GetComponentLocation();
		// 총구 정면으로 1.5미터 검사
		FVector MuzzleEnd = MuzzleStart + (FireStartLocation->GetForwardVector() * 150.0f);

		FCollisionQueryParams MuzzleParams;
		MuzzleParams.AddIgnoredActor(this);
		MuzzleParams.AddIgnoredActor(GetOwner());

		bool bMuzzleHit = GetWorld()->LineTraceSingleByChannel(
			MuzzleHit,
			MuzzleStart,
			MuzzleEnd,
			ECC_Visibility,
			MuzzleParams
		);

		if (bMuzzleHit && MuzzleHit.GetActor() && MuzzleHit.GetActor()->IsA(APawn::StaticClass()))
		{
			HitResult = MuzzleHit;
			bHit = true;

			// 디버그용 (보라색 선)
			if (bShowDebugTrace)
			{
				DrawDebugLine(GetWorld(), MuzzleStart, MuzzleEnd, FColor::Magenta, false, 5.0f, 0, 0.5f);
			}
		}
	}

	// 디버그용 선 추가 
	if (bShowDebugTrace)
	{
		FVector BeamEnd = bHit ? HitResult.ImpactPoint : End;

		// 실제 트레이스선(초록색, 카메라 중앙)
		DrawDebugLine(
			GetWorld(),
			Start,
			BeamEnd,
			FColor::Green,
			false,
			5.0f,
			0,
			0.5f
		);

		// 눈속임선(빨간색, 총구에서 카메라 중앙)
		if (FireStartLocation)
		{
			FVector MuzzleLocation = FireStartLocation->GetComponentLocation();

			DrawDebugLine(
				GetWorld(),
				MuzzleLocation,
				BeamEnd,
				FColor::Red,
				false,
				5.0f,
				0,
				0.5f
			);
		}
	}

	// 트레이스 이펙트 스폰
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
		ApplyDamageToTarget(HitResult.GetActor(), HitResult,BaseDamage);
		PlayImpactEffects(HitResult);
	}
}

void ASanzoGun::ApplyWeaponStatUpgrade(EUpgradeType Type, float Value)
{
	Super::ApplyWeaponStatUpgrade(Type, Value);

	switch (Type)
	{
	case EUpgradeType::FireRate:
		FireRate += Value;
		break;

	default:
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("샤갈! 이상한값이 발생했어요!"));
		break;

		
	}
	
}

void ASanzoGun::AddAmmo(int32 Amount)
{
	CurrentAmmo += Amount;
	OnAmmoChanged.Broadcast();
	UE_LOG(LogTemp, Log, TEXT("Current Ammo: %d"), CurrentAmmo);
}

#pragma region DataForHUD

FText ASanzoGun::GetAmmoTextForHUD() const
{
	return FText::AsNumber(CurrentAmmo);
}
#pragma endregion 이준로