#include "Weapon/SanzoGun.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Components/ArrowComponent.h"
#include "TimerManager.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AI/SanzoEnemyBase.h"
#include "Weapon/SanzoProjectile.h"

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
	// 미사일 스폰 위치 담당 컴포넌트 메시와 연결
	MissileSpawnLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("MissileSpawnLocation"));
	MissileSpawnLocation->SetupAttachment(WeaponMesh);
	MissileSpawnLocation->SetRelativeRotation(FRotator(45.0f, 90.0f, 0.0f));

	HomingMissileChance = 0.0f;
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

	// 호밍 미사일 발사 시스템, 발사 확률이 0보다 크고 발사 확률에 걸렸을 때만 실행
	if (HomingMissileChance > 0.0f && FMath::RandRange(1.0f, 100.0f) <= HomingMissileChance)
	{
		if (HomingProjectileClass && FireStartLocation)
		{
			// 화면에 보이는 랜덤한 적 저장
			ASanzoEnemyBase* RandomTarget = FindRandomVisibleEnemy();

			// 적이 화면에 보이면 위치 저장
			if (RandomTarget)
			{
				if (MissileSpawnLocation)
				{
					FVector SpawnLocation = MissileSpawnLocation->GetComponentLocation();
					FRotator SpawnRotation = MissileSpawnLocation->GetComponentRotation();
					// 발사할 때마다 좌우 랜덤 각도로 발사
					SpawnRotation.Yaw += FMath::RandRange(-35.0f, 35.0f);

					FActorSpawnParameters SpawnParams;
					SpawnParams.Owner = this;
					SpawnParams.Instigator = Cast<APawn>(GetOwner());
					SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

					// 호밍 미사일 스폰
					ASanzoProjectile* Missile = GetWorld()->SpawnActor<ASanzoProjectile>(HomingProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);

					if (Missile)
					{
						// 미사일 데미지 설정 (후에 조정 예정)
						Missile->SetArrowDamage(BaseDamage * 2.0f);

						// 미사일의 타겟 설정 (기본 타겟값은 루트 컴포넌트로 설정)
						USceneComponent* TargetComp = RandomTarget->GetRootComponent();

						// 적의 컴포넌트 중에 태그로 LockOn 태그 달고 있는 컴포넌트 찾기
						TArray<USceneComponent*> EnemyComps;
						RandomTarget->GetComponents(USceneComponent::StaticClass(), EnemyComps);
						for (USceneComponent* Comp : EnemyComps)
						{
							if (Comp->ComponentHasTag(FName("LockOn")))
							{
								// 찾았으면 타겟 교체
								TargetComp = Comp; 
								break;
							}
						}
						Missile->SetHomingTarget(TargetComp);
					}
				}
			}
		}
	}
}

void ASanzoGun::ApplyWeaponStatUpgrade(EUpgradeType Type, float Value)
{
	Super::ApplyWeaponStatUpgrade(Type, Value);

	switch (Type)
	{
	case EUpgradeType::FireRate:
		FireRate = FMath::Max(0.07, FireRate - Value);
		break;

	case EUpgradeType::HomingMissile:
		// Value 값에 들어오는 숫자대로 퍼센트가 오름 (ex) Value = 5.0 이면 생성 확률 5% 증가)
		HomingMissileChance += Value;
		break;

	default:
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("샤갈! 이상한값이 발생했어요!"));
		break;

		
	}
	
}
// 호밍 시스템을 위한 시야 안의 랜덤한 적을 뽑는 함수
ASanzoEnemyBase* ASanzoGun::FindRandomVisibleEnemy()
{
	// 무기 주인 없으면 실행 X
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return nullptr;

	// 컨트롤러 없으면 실행 x
	APlayerController* PlayerController = Cast<APlayerController>(OwnerPawn->GetController());
	if (!PlayerController) return nullptr;
	// 플레이어 컨트롤러에서 뷰포트 X,Y 사이즈 가져와서 저장
	int32 ViewportSizeX, ViewportSizeY;
	PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);

	// 월드의 모든 액터 저장할 배열 선언 후 저장
	TArray<AActor*> FoundEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASanzoEnemyBase::StaticClass(), FoundEnemies);

	// 화면에 보이는 적만 저정할 배열 선언
	TArray<ASanzoEnemyBase*> VisibleEnemies;

	for (AActor* Actor : FoundEnemies)
	{
		ASanzoEnemyBase* Enemy = Cast<ASanzoEnemyBase>(Actor);
		// 적이 아니거나 죽었으면 스킵하고 다음 for문
		if (!Enemy || Enemy->IsDead()) continue;

		FVector2D ScreenPos;
		// 모니터 화면 안에 있는지 확인
		if (PlayerController->ProjectWorldLocationToScreen(Enemy->GetActorLocation(), ScreenPos))
		{
			// 시야에 보이는지 확인
			if (ScreenPos.X >= 0 && ScreenPos.X <= ViewportSizeX &&
				ScreenPos.Y >= 0 && ScreenPos.Y <= ViewportSizeY)
			{
				FHitResult HitResult;
				FCollisionQueryParams QueryParams;
				QueryParams.AddIgnoredActor(this);
				QueryParams.AddIgnoredActor(OwnerPawn);

				if (GetWorld()->LineTraceSingleByChannel(HitResult, OwnerPawn->GetActorLocation(), Enemy->GetActorLocation(), ECC_Visibility, QueryParams))
				{
					if (HitResult.GetActor() == Enemy)
					{
						// 화면에 잡힌게 적이 맞으면 배열에 추가
						VisibleEnemies.Add(Enemy);
					}
				}
			}
		}
	}

	// 배열이 비어있지 않으면 그 중 하나 랜덤으로 뽑기
	if (VisibleEnemies.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, VisibleEnemies.Num() - 1);
		return VisibleEnemies[RandomIndex];
	}

	// 다 해당안되면 nullptr 반환
	return nullptr;
}

void ASanzoGun::AddAmmo(int32 Amount)
{
	CurrentAmmo += Amount;
	OnAmmoChanged.Broadcast();
	UE_LOG(LogTemp, Log, TEXT("Current Ammo: %d"), CurrentAmmo);
}

void ASanzoGun::SetCurrentAmmo(int32 Amount)
{
	Super::SetCurrentAmmo(Amount);
	
	OnAmmoChanged.Broadcast();
}

#pragma region DataForHUD

FText ASanzoGun::GetAmmoTextForHUD() const
{
	return FText::AsNumber(CurrentAmmo);
}
#pragma endregion 이준로