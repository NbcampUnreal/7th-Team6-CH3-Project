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
}

void ASanzoBow::StopFire()
{
	Super::StopFire();
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
				Arrow->SetDamage(BaseDamage);
			}
		}
	}

}