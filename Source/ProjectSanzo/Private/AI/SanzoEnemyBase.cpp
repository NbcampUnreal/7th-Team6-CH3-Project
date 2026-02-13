// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/SanzoEnemyBase.h"
#include "AI/SanzoAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "Common/SanzoLog.h"
#include "BrainComponent.h"
#include "Components/ProgressBar.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Stage/SanzoRoomBase.h"

ASanzoEnemyBase::ASanzoEnemyBase()
{
  PrimaryActorTick.bCanEverTick = false;

  AIControllerClass = ASanzoAIController::StaticClass();
  // 맵에 놓거나 스폰되면 자동 빙의
  AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

  GetCharacterMovement()->MaxWalkSpeed = 350.f;
  // 이동 방향을 보도록 설정
  GetCharacterMovement()->bOrientRotationToMovement = true;

  GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);
	
#pragma region OverHeadUI
	
	OverHeadHPBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverHeadWidget"));
	
	OverHeadHPBar->SetupAttachment(GetCapsuleComponent());
	
	OverHeadHPBar->SetRelativeLocation(FVector(0.0f, 0.0f, 115.0f));
	
	OverHeadHPBar->SetRelativeScale3D(FVector(.15f,0.15f,0.15f));
	
#pragma endregion 이준로
	
}

void ASanzoEnemyBase::BeginPlay()
{
  Super::BeginPlay();
  CurrentHP = MaxHP;
  bIsDead = false;
	
#pragma region OverHeadUI
	
	UpdateOverHeadHPBar();
	
	GetWorldTimerManager().SetTimer(
		OverHeadHPBarUpdateTimerHandle,
		this,
		&ASanzoEnemyBase::MakeOverHeadHPBar3D,
		0.01f,
		true
	);
#pragma endregion 이준로
	
#pragma region Find RoomBase
  ASanzoRoomBase* Found = Cast<ASanzoRoomBase>(UGameplayStatics::GetActorOfClass(GetWorld(), ASanzoRoomBase::StaticClass()));
  if (Found)
  {
    CurrentRoom = Found;
  }
#pragma endregion 최윤서
}

//void ASanzoEnemyBase::Tick(float DeltaTime)
//{
//  Super::Tick(DeltaTime);
//
//}

void ASanzoEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
  Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool ASanzoEnemyBase::IsDead() const
{
  return bIsDead;
}

float ASanzoEnemyBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
  if (bIsDead) return 0.f;

  float FinalDamage = DamageAmount;
  CurrentHP = FMath::Clamp(CurrentHP - FinalDamage, 0.f, MaxHP);

	//HP바 갱신
	UpdateOverHeadHPBar();
	
  if (GEngine)
  {
    FString Message = FString::Printf(TEXT("[Enemy Recieved] Damage: %.1f | HP Left: %.1f"),
      FinalDamage,
      CurrentHP);

    GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, Message);
  }

  UE_LOG(LogKDJ, Warning, TEXT("Enemy Took Damage: %f"), FinalDamage);

  // 사망 처리
  if (CurrentHP <= 0.f && !bIsDead)
  {
    Die();
  }

  return FinalDamage;
}

void ASanzoEnemyBase::Die()
{
  bIsDead = true;
  UE_LOG(LogKDJ, Error, TEXT("Enemy Died! Engaging Ragdoll."));

  // TO-DO: Room에 사망 알림
  #pragma region Call RoomBase
    // 사망 SanzoRoomBase::OnEnemyKilled() 호출
    if (CurrentRoom)
    {
      UE_LOG(LogCYS, Warning, TEXT("EB: RoomBase에 사망 알림"));
      CurrentRoom->OnEnemyKilled();
    }
  #pragma endregion 최윤서

  // AI 로직 정지
  AAIController* AICon = Cast<AAIController>(GetController());
  if (AICon && AICon->GetBrainComponent())
  {
    AICon->GetBrainComponent()->StopLogic("Dead");
  }
 #pragma region OverHeadUI
	//HPBar 숨기기
	if (OverHeadHPBar)
	{
		OverHeadHPBar->SetVisibility(false);
	}
	//TimerHandle 초기화
	GetWorldTimerManager().ClearTimer(OverHeadHPBarUpdateTimerHandle);
	
 #pragma endregion 이
  // 충돌 끄기 및 래그돌(물리) 실행
  GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
  GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
  GetMesh()->SetSimulatePhysics(true);

  // 일정 시간 후 액터 제거
  SetLifeSpan(5.f);
}

void ASanzoEnemyBase::Attack()
{
  if (AttackMontage && !bIsDead)
  {
    PlayAnimMontage(AttackMontage);
  }
}

void ASanzoEnemyBase::Fire()
{
  if (ProjectileClass && !bIsDead)
  {
    FVector SpawnLocation =
      GetActorLocation() + (GetActorForwardVector() * 100.f)
      + FVector(0.f, 0.f, 50.f);
    FRotator SpawnRotation = GetActorRotation();

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = this;

    GetWorld()->SpawnActor<AActor>(
      ProjectileClass,
      SpawnLocation,
      SpawnRotation,
      SpawnParams);
    UE_LOG(LogKDJ, Log, TEXT("Enemy Fired Projectile!"));
  }
}
#pragma region OverHeadUI

void ASanzoEnemyBase::UpdateOverHeadHPBar()
{
	if (!OverHeadHPBar) return;
	
	UUserWidget* OverHeadHPBarInstance = OverHeadHPBar->GetUserWidgetObject();
	if (!OverHeadHPBarInstance) return;
	
	if (UProgressBar* HealthBar = Cast<UProgressBar>(OverHeadHPBarInstance->GetWidgetFromName(TEXT("HealthBar"))))
	{
		if (MaxHP > 0.f)
		{
			float HealthPercent = CurrentHP / MaxHP;
			if (HealthPercent >= 1.f)
			{
				OverHeadHPBar->SetVisibility(false);
			}
			else
			{
				OverHeadHPBar->SetVisibility(true);
			}
			HealthBar->SetPercent(HealthPercent);
		}
	}	
}

void ASanzoEnemyBase::MakeOverHeadHPBar3D()
{
	if (!OverHeadHPBar) return;
	
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (!CameraManager) return;
	
	FVector CameraLocation = CameraManager->GetCameraLocation();
	FVector WidgetLocation = OverHeadHPBar->GetComponentLocation();
	
	FRotator LookCameraRotation = UKismetMathLibrary::FindLookAtRotation(WidgetLocation, CameraLocation);
	OverHeadHPBar->SetWorldRotation(LookCameraRotation);
}
#pragma endregion 이준로