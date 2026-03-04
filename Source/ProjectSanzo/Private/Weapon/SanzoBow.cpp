#include "Weapon/SanzoBow.h"
#include "Weapon/SanzoProjectile.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "GameplayTagAssetInterface.h"
#include "Common/SanzoGameplayTag.h"
ASanzoBow::ASanzoBow()
{
	// 시위에 보일 가짜화살 생성
	DummyArrowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyArrowMesh"));
	DummyArrowMesh->SetupAttachment(WeaponMesh, TEXT("StringSocket"));
	DummyArrowMesh->SetCollisionProfileName(TEXT("NoCollision")); // 콜리전 판정 없애기
	DummyArrowMesh->SetHiddenInGame(true); // 평소에는 안보이게 설정

	// 소켓 이름 기본값 설정 (후에 에디터에서 수정 가능)
	StringSocketName = TEXT("StringSocket");

	BaseDamage = 40.0f;
  
#pragma region DataForHUD
	//Tick 사용
	PrimaryActorTick.bCanEverTick = true;
	//스폰시에는 Tick 끄고 스폰.
	PrimaryActorTick.bStartWithTickEnabled = false;
	
#pragma endregion 이준로
	
}

void ASanzoBow::StartFire()
{
	Super::StartFire();

	//PercentBar 방송용 Tick 활성화 - 작업자:이준로
	SetActorTickEnabled(true);
	
	// 차징 시작한 현재 시간 기록
	ChargeStartTime = GetWorld()->GetTimeSeconds();
	//PercentBar 차오르기 시작 bool 값 변경 - 작업자:이준로
	bIsCharging = true;

	// 차징 시작하면 더미 화살 보여주기
	if (DummyArrowMesh)
	{
		DummyArrowMesh->SetHiddenInGame(false);
	}

	// 활든 사람 몽타지 재생
	PlayFireEffects();
	// 활 애니메이션 재생
	if (WeaponMesh && BowDrawAnim)
	{
		// 활을 발사하는 사람 몽타지 중 Draw 섹션의 재생 길이를 가져옴
		int32 SectionIndex = CharacterFireMontage->GetSectionIndex(FName("Default"));
		float CurrentSectionLength = CharacterFireMontage->GetSectionLength(SectionIndex);

		// 활 애니메이션 길이 가져옴
		float BowAnimOriginalLength = BowDrawAnim->GetPlayLength();

		// 활 애니메이션을 몇 배속 해야지 사람 모션과 재생 시간이 같이지는지 계산
		float SyncPlayRate = (CurrentSectionLength > 0.f) ? (BowAnimOriginalLength / CurrentSectionLength) : 1.0f;

		// 활 애니메이션 재생 속도 조정
		WeaponMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
		WeaponMesh->PlayAnimation(BowDrawAnim, false);
		WeaponMesh->SetPlayRate(SyncPlayRate);
	}
	
	// 활 당기는 사운드 재생
	if (DrawStartSound)
	{
		DrawStartAudioComp = UGameplayStatics::SpawnSoundAttached(DrawStartSound, WeaponMesh, StringSocketName);
	}
	// 최대 차지 시간 끝나면 사운드 바꾸는 함수 실행하도록 예약
	GetWorldTimerManager().SetTimer(
		SwitchSoundTimer,
		this,
		&ASanzoBow::SwitchToLoopSound,
		MaxChargeTime,
		false
	);
}

void ASanzoBow::StopFire()
{
	Super::StopFire();
	
	if (IGameplayTagAssetInterface* TagCheak = Cast<IGameplayTagAssetInterface>(GetOwner()))
	{
		if (TagCheak->HasMatchingGameplayTag(SanzoTags::HitReaction) ||
			TagCheak->HasMatchingGameplayTag(SanzoTags::Swap)
			)
		{
			ChargeStartTime = GetWorld()->GetTimeSeconds();
			ChargePercent = 0;
			if (OnChargePercentChanged.IsBound())
			{
				OnChargePercentChanged.Broadcast(0.0f);
			}
			return;
		}
	}

#pragma region DataForHUD
	
	//PercentBar 방송용 Tick 비활성화
	SetActorTickEnabled(false);
	//PercentBar 차오르기 종료 bool 값 변경
	bIsCharging = false;
	//PercentBar 초기화
	if (OnChargePercentChanged.IsBound())
	{
		OnChargePercentChanged.Broadcast(0.0f);
	}
	
#pragma endregion 이준로



	// 치지가 끝나기 전에 발사가 호출되면 차지 사운드 예약한 타이머 끄기
	GetWorldTimerManager().ClearTimer(SwitchSoundTimer);
	// 재생되던 드로우 사운드 끄고 혹시 있을 차지 사운드도 끄기
	if (DrawStartAudioComp && DrawStartAudioComp->IsPlaying()) DrawStartAudioComp->Stop();
	if (DrawLoopAudioComp && DrawLoopAudioComp->IsPlaying()) DrawLoopAudioComp->Stop();
	
	// 화살 발사하면 더미화살 다시 안보이게 변경
	if (DummyArrowMesh)
	{
		DummyArrowMesh->SetHiddenInGame(true);
	}

	// 활 애니메이션 초기화
	if (WeaponMesh)
	{
		WeaponMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	}

	// 활 캐릭터 애니메이션 풀기
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (OwnerCharacter)
	{
		UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
		if (AnimInstance && CharacterFireMontage)
		{
			// 활 발사 몽타지의  Fire 섹션으로 점프
			AnimInstance->Montage_JumpToSection(FName("Fire"), CharacterFireMontage);
		}
	}

	// 누르고 있던 시간 계산
	float ChargeDuration = GetWorld()->GetTimeSeconds() - ChargeStartTime;
	// 차징값에 최대값을 정하기 위한 계산
	float ClampedDuration = FMath::Clamp(ChargeDuration, 0.0f, MaxChargeTime);
	// 구한 값을 0 ~ 1.0 사이의 퍼센티지로 계산
	ChargePercent = ClampedDuration / MaxChargeTime;

	// 일정 차징 시간 미만이면 화살 발사 X
	if (ChargePercent < 0.2f)
	{
		// 더미화살 없애기
		if (DummyArrowMesh)
		{
			DummyArrowMesh->SetHiddenInGame(true);
		}

		// 당기던 애니메이션(몽타주) 중단하기
		if (WeaponMesh)
		{
			WeaponMesh->Stop();
		}

		return;
	}

	Fire();
}

void ASanzoBow::CancelCharge()
{
	if (!bIsCharging) return;

	bIsCharging = false;

	// Tick 비활성화
	SetActorTickEnabled(false);

	if (DrawStartAudioComp && DrawStartAudioComp->IsPlaying())
	{
		DrawStartAudioComp->Stop();
	}
	if (DrawLoopAudioComp && DrawLoopAudioComp->IsPlaying())
	{
		DrawLoopAudioComp->Stop();
	}
	// 차징 UI 초기화
	if (OnChargePercentChanged.IsBound())
	{
		OnChargePercentChanged.Broadcast(0.0f);
	}

	if (DummyArrowMesh)
	{
		DummyArrowMesh->SetHiddenInGame(true);
	}
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
			// 사라진 사거리 대신 사용할 트레이스 끝점 
			float TraceDistance = 100000.f;
			// 카메라 정면으로부터 떨어진 끝점 값 저장
			FVector TraceEnd = CameraLocation + (CameraRotation.Vector() * TraceDistance);

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

			// 화살 업그레이드 되었는지 판단
			int32 ArrowCount = bIsMultiShotEnabled ? 3 : 1;

			for (int32 i = 0; i < ArrowCount; i++)
			{

				FRotator CurrentArrowRotation = SpawnRotation;

				// 업그레이드 되었을 때만 각도 틀기
				if (bIsMultiShotEnabled)
				{
					float AngleOffset = (i - 1) * 5.0f;
					CurrentArrowRotation.Yaw += AngleOffset;
				}

				ASanzoProjectile* Arrow = World->SpawnActor<ASanzoProjectile>(
					ProjectileClass,
					SpawnLocation,
					CurrentArrowRotation,
					SpawnParams);

				if (Arrow)
				{
					float MinArrowDamage = BaseDamage * 0.2;
					float MaxArrowDamage = BaseDamage;
					// 화살의 Speed,Damage 값 계산 (최소값 + ( 최대값 - 최소값 ) * 차징퍼센트 ) 방식
					float FinalSpeed = FMath::Lerp(MinArrowSpeed, MaxArrowSpeed, ChargePercent);
					float FinalDamage = FMath::Lerp(MinArrowDamage, MaxArrowDamage, ChargePercent);
					// 화살에 계산한 속도,데미지 전달
					Arrow->SetArrowSpeed(FinalSpeed);
					Arrow->SetArrowDamage(FinalDamage);

					// 다 하고 발사 사운드 재생
					if (FireSound)
					{
						FireAudioComp = UGameplayStatics::SpawnSoundAttached(FireSound, WeaponMesh, StringSocketName);
					}
				}
			}
		}
	}

}

void ASanzoBow::ApplyWeaponStatUpgrade(EUpgradeType Type, float Value)
{
	Super::ApplyWeaponStatUpgrade(Type, Value);

  switch (Type)
  {
	case EUpgradeType::MaxChargeTime:
		MaxChargeTime = FMath::Max(0.2, MaxChargeTime - Value);
		break;

	case EUpgradeType::MultiShot:
		if (Value > 0.0f)
		{
			bIsMultiShotEnabled = true;
			BaseDamage = (BaseDamage / 4) * 3;
		}
		break;
  default:
    break;
  }
}

#pragma region DataForHUD

void ASanzoBow::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (bIsCharging)
	{
		float CurrentDuration = GetWorld()->GetTimeSeconds() - ChargeStartTime;
		float CurrentPercent = FMath::Clamp(CurrentDuration / MaxChargeTime, 0.0f, 1.0f);
		
		if (OnChargePercentChanged.IsBound())
		{
			OnChargePercentChanged.Broadcast(CurrentPercent);
		}
	}
}

FText ASanzoBow::GetAmmoTextForHUD() const
{
	return FText::FromString(TEXT("무한"));
}
	
#pragma endregion 이준로

void ASanzoBow::SwitchToLoopSound()
{
	// 차징 사운드 재생되기 전 원래 나오던 드로우 사운드 재생 끝냄
	if (DrawStartAudioComp && DrawStartAudioComp->IsPlaying())
	{
		DrawStartAudioComp->Stop();
	}

	// 차지 했을 때 나와야할 사운드 재생
	if (DrawLoopSound)
	{
		DrawLoopAudioComp = UGameplayStatics::SpawnSoundAttached(DrawLoopSound, WeaponMesh, StringSocketName);
	}
}