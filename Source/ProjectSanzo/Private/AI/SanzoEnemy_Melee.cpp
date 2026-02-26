#include "AI/SanzoEnemy_Melee.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Common/SanzoGameplayTag.h"
#include "AI/Components/SanzoEnemyStunComponent.h" 
#include "GameplayTagAssetInterface.h"

ASanzoEnemy_Melee::ASanzoEnemy_Melee()
{
  // 근접 적 기본 스탯 세팅
  MaxHP = 120.f;
  AttackRange = 180.f;
  MeleeDamage = 30.f;
  Exp = 30.f;
}

void ASanzoEnemy_Melee::Attack()
{
  Super::Attack();
#pragma region Sound
  if (MeleeAttackSound)
    {
    UGameplayStatics::PlaySoundAtLocation(
      this, 
      MeleeAttackSound, 
      GetActorLocation(),
      1.f,
      FMath::FRandRange(0.9f, 1.1f), // 피치 변동
      0.f,
      EnemyAttenuation
    );
  }
#pragma endregion 최윤서
}