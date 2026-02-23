#include "Weapon/SanzoBow.h"
#include "Weapon/SanzoProjectile.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"

ASanzoBow::ASanzoBow()
{

}

void ASanzoBow::StartFire()
{
	Super::StartFire();

	// 차징 시작한 현재 시간 기록
	ChargeStartTime = GetWorld()->GetTimeSeconds();
}

void ASanzoBow::StopFire()
{
	Super::StopFire();

	// 누르고 있던 시간 계산
	float ChargeDuration = GetWorld()->GetTimeSeconds() - ChargeStartTime;
	// 차징값에 최대값을 정하기 위한 계산
	float ClampedDuration = FMath::Clamp(ChargeDuration, 0.0f, MaxChargeTime);
	// 구한 값을 0 ~ 1.0 사이의 퍼센티지로 계산
	ChargePercent = ClampedDuration / MaxChargeTime;

	Fire();
}

void ASanzoBow::Fire()
{
	if (ProjectileClass && FireStartLocation)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			// 화살 스폰 위치
			FVector SpawnLocation = FireStartLocation->GetComponentLocation();
			// 목표 지점 넣을 변수 선언
			FVector TargetPoint = FVector::ZeroVector;

			APawn* OwnerPawn = Cast<APawn>(GetOwner());
			if (!OwnerPawn) return;
			AController* OwnerController = OwnerPawn->GetController();
			if (!OwnerController) return;

			// 카메라에서 정보값 가져오기
			FVector CameraLocation;
			FRotator CameraRotation;
			OwnerController->GetPlayerViewPoint(CameraLocation, CameraRotation);
			// 카메라 정면으로부터 사거리만큼 떨어진 끝점 저장
			FVector TraceEnd = CameraLocation + (CameraRotation.Vector() * MaxRange);

			FHitResult HitResult;
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(this); // 활 무시
			QueryParams.AddIgnoredActor(GetInstigator()); // 플레이어 무시

			bool bHit = World->LineTraceSingleByChannel(
				HitResult,
				CameraLocation,
				TraceEnd,
				ECC_Visibility,
				QueryParams
			);

			// 목표 지점 결정
			if (bHit)
			{
				TargetPoint = HitResult.ImpactPoint; // 맞은 곳
			}
			else
			{
				TargetPoint = TraceEnd; // 아무것도 안 맞았으면 허공 끝점
			}

			// 화살 스폰 위치에서 목표 지점을 바라보는 회전값 계산
			FRotator SpawnRotation = UKismetMathLibrary::FindLookAtRotation(SpawnLocation, TargetPoint);

			// 화살이 너무 낮게 가는 것 같아서 발사 각도 올림(후에 수정할 수도 있음)
			float PitchOffset = 1.0f;
			SpawnRotation.Pitch += PitchOffset;

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			// 새로 계산한 회전값으로 화살 소환
			ASanzoProjectile* Arrow = World->SpawnActor<ASanzoProjectile>(
				ProjectileClass,
				SpawnLocation,
				SpawnRotation,
				SpawnParams);

			if (Arrow)
			{
				// 화살의 Speed,Damage 값 계산 (최소값 + ( 최대값 - 최소값 ) * 차징퍼센트 ) 방식
				float FinalSpeed = FMath::Lerp(MinArrowSpeed, MaxArrowSpeed, ChargePercent);
				float FinalDamage = FMath::Lerp(MinArrowDamage, MaxArrowDamage, ChargePercent);
				// 화살에 계산한 속도,데미지 전달
				Arrow->SetArrowSpeed(FinalSpeed);
				Arrow->SetArrowDamage(FinalDamage);
			}
		}
	}

}