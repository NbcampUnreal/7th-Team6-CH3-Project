// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/SanzoEnemyBase.h"
#include "AI/SanzoAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "Common/SanzoLog.h"
#include "Kismet/GameplayStatics.h"
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
}

void ASanzoEnemyBase::BeginPlay()
{
  Super::BeginPlay();
  CurrentHP = MaxHP;

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
  return CurrentHP <= 0.f;
}

float ASanzoEnemyBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
  float FinalDamage = DamageAmount;

  CurrentHP = FMath::Clamp(CurrentHP - FinalDamage, 0.f, MaxHP);

  if (GEngine)
  {
    FString Message = FString::Printf(TEXT("[Enemy Recieved] Damage: %.1f | HP Left: %.1f"),
      FinalDamage,
      CurrentHP);

    GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, Message);
  }

  UE_LOG(LogTemp, Warning, TEXT("Enemy Took Damage: %f"), FinalDamage);

  // 사망 처리
  if (CurrentHP <= 0.f)
  {
    // TO-DO: 사망 처리 함수 호출 (SanzoRoomBase::OnEnemyKilled() 호출, 사망 애니메이션 몽타주 재생 등)
    UE_LOG(LogTemp, Error, TEXT("Enemy Died!"));

#pragma region Call RoomBase
    // 사망 SanzoRoomBase::OnEnemyKilled() 호출
    if (CurrentRoom)
    {
      UE_LOG(LogCYS, Warning, TEXT("EB: RoomBase에 사망 알림"));
      CurrentRoom->OnEnemyKilled();
    }
#pragma endregion 최윤서
    // 캡슐 충돌 끄고 래그돌 실행
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetMesh()->SetSimulatePhysics(true);
  }

  return FinalDamage;
}
