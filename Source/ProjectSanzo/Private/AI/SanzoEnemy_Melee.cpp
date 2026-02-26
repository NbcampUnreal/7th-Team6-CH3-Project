#include "AI/SanzoEnemy_Melee.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Common/SanzoGameplayTag.h"
#include "AI/Components/SanzoEnemyStunComponent.h" 

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
}