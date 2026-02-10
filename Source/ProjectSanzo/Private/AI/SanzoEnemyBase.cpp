// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/SanzoEnemyBase.h"
#include "AI/SanzoAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"

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
  bool bIsHeadShot = false;

  // PointDamage 이벤트인지 확인
  if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
  {
    // 이벤트를 캐스팅해서 상세 정보를 가져옴
    const FPointDamageEvent* PointEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
    FName HitBone = PointEvent->HitInfo.BoneName;

    // 헤드샷 판별
    if (HitBone == HeadBoneName)
    {
      bIsHeadShot = true;
      FinalDamage *= HeadShotMultiplier;
    }
  }

  // 체력 감소
  CurrentHP = FMath::Clamp(CurrentHP - FinalDamage, 0.f, MaxHP);

  // 디버그 로그 출력
  if (GEngine)
  {
    FString Message = FString::Printf(TEXT("[%s] Hit! Bone: %s | Damage: %.1f | HP: %.1f"),
      bIsHeadShot ? TEXT("HEADSHOT!!!") : TEXT("Body"), // 헤드샷 여부
      *static_cast<const FPointDamageEvent*>(&DamageEvent)->HitInfo.BoneName.ToString(),
      FinalDamage, // 최종 데미지
      CurrentHP   // 남은 체력
    );

    // 헤드샷이면 빨간색, 아니면 노란색으로 2초간 표시
    FColor TextColor = bIsHeadShot ? FColor::Red : FColor::Yellow;
    GEngine->AddOnScreenDebugMessage(-1, 2.f, TextColor, Message);
  }

  UE_LOG(
    LogTemp,
    Warning,
    TEXT("Enemy Damaged: %f (Headshot: %d)"),
    FinalDamage,
    bIsHeadShot
  );

  // 사망 처리
  if (CurrentHP <= 0.f)
  {
    UE_LOG(LogTemp, Error, TEXT("Enemy Died!"));
  }

  return FinalDamage;
}
