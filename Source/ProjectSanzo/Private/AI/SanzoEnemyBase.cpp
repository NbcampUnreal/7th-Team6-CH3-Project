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
#include "Components/SkeletalMeshComponent.h"
#include "Perception/AISense_Damage.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Chaos/Deformable/MuscleActivationConstraints.h"
#include "Dataflow/DataflowContent.h"
#include "UI/SanzoEnemyOverHeadWidget.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AI/Components/SanzoEnemyStunComponent.h"
#include "Character/Interface/SanzoRewardReceiverInterface.h"
#include "UI/SanzoBossOverheadWidget.h"

ASanzoEnemyBase::ASanzoEnemyBase()
{
  AIControllerClass = ASanzoAIController::StaticClass();
  // 맵에 놓거나 스폰되면 자동 빙의
  AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

  GetCharacterMovement()->MaxWalkSpeed = 350.f;
  // 이동 방향을 보도록 설정
  GetCharacterMovement()->bOrientRotationToMovement = true;

  GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

  GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);

  GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

  // 적 무기 컴포넌트 설정 (스켈레탈 매시)
  WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
  // 오른손 뼈(hand_r)에 무기를 기본적으로 부착
  WeaponMesh->SetupAttachment(GetMesh(), TEXT("hand_r"));
  WeaponMesh->SetCollisionProfileName(TEXT("NoCollision"));

  // 적 무기 컴포넌트 설정 (스태틱 매시)
  StaticWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticWeaponMesh"));
  StaticWeaponMesh->SetupAttachment(GetMesh(), TEXT("hand_r"));
  StaticWeaponMesh->SetCollisionProfileName(TEXT("NoCollision"));

#pragma region OverHeadUI

  OverHeadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverHeadWidget"));

  OverHeadWidget->SetupAttachment(GetCapsuleComponent());

  OverHeadWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 115.0f));

  OverHeadWidget->SetRelativeScale3D(FVector(.3f, 0.3f, 0.3f));

  OverHeadWidget->SetDrawSize(FVector2D(500, 900));

#pragma endregion 이준로

#pragma region ProximitySensor
  // 육감 구체 생성 및 세팅
  ProximitySensor = CreateDefaultSubobject<USphereComponent>(TEXT("ProximitySensor"));
  ProximitySensor->SetupAttachment(RootComponent);
  ProximitySensor->SetSphereRadius(350.f);
  ProximitySensor->SetCollisionProfileName(TEXT("Trigger"));

  // 오버랩 이벤트 연결
  ProximitySensor->OnComponentBeginOverlap.AddDynamic(this, &ASanzoEnemyBase::OnProximityOverlap);
#pragma endregion 김동주

#pragma region StunComponent
  StunComponent = CreateDefaultSubobject<USanzoEnemyStunComponent>(TEXT("StunComponent"));
#pragma endregion 김동주

}

void ASanzoEnemyBase::BeginPlay()
{
  Super::BeginPlay();
  CurrentHP = MaxHP;
  bIsDead = false;

#pragma region StunComponent
  if (StunComponent)
  {
    StunComponent->OnStunCountChanged.AddDynamic(this, &ASanzoEnemyBase::OnStunCountChangedCallback);
    StunComponent->OnStunStateEntered.AddDynamic(this, &ASanzoEnemyBase::OnStunEnteredCallback);
    StunComponent->OnStunStateRecovered.AddDynamic(this, &ASanzoEnemyBase::OnStunRecoveredCallback);
    StunComponent->OnParried.AddDynamic(this, &ASanzoEnemyBase::OnParriedCallback);
  }
#pragma endregion 김동주

#pragma region OverHeadUI

  if (OverHeadWidget)
  {
    UUserWidget* OverHeadWidgetInstance = OverHeadWidget->GetUserWidgetObject();

    USanzoEnemyOverHeadWidget* CurrentWidget = Cast<USanzoEnemyOverHeadWidget>(OverHeadWidgetInstance);

    if (CurrentWidget)
    {
      OnEnemyDataChanged.AddDynamic(CurrentWidget, &USanzoEnemyOverHeadWidget::UpdateOverHeadWidget);
    }
  	
  	//보스전용 Delegate
  	USanzoBossOverheadWidget* BossOverheadWidget = Cast<USanzoBossOverheadWidget>(OverHeadWidgetInstance);
  	
  	if (BossOverheadWidget)
  	{
  		OnEnemyDataChanged.AddDynamic(BossOverheadWidget, &USanzoBossOverheadWidget::UpdateOverHeadWidget);
  	}
    BroadCastAllData();
  }

  GetWorldTimerManager().SetTimer(
    OverHeadWidgetUpdateTimerHandle,
    this,
    &ASanzoEnemyBase::MakeOverHeadWidget3D,
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


void ASanzoEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
  Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool ASanzoEnemyBase::IsDead() const
{
  return bIsDead;
}

float ASanzoEnemyBase::GetAttackRange() const
{
  return AttackRange;
}

#pragma region EnemyTakeDamage
float ASanzoEnemyBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
  float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

  if (bIsDead || ActualDamage <= 0.f)
  {
    return 0.f;
  }

  CurrentHP = FMath::Clamp(CurrentHP - ActualDamage, 0.f, MaxHP);

  //HP바 갱신
  BroadCastAllData();
  if (EventInstigator && EventInstigator->GetPawn())
  {
    UAISense_Damage::ReportDamageEvent(
      GetWorld(),
      this,
      EventInstigator->GetPawn(),
      ActualDamage,
      EventInstigator->GetPawn()->GetActorLocation(),
      GetActorLocation()
    );
  }

  // 사망 처리
  if (CurrentHP <= 0.f && !bIsDead)
  {
    Die();

    //형백
    if (ISanzoRewardReceiverInterface* Reward = Cast<ISanzoRewardReceiverInterface>(EventInstigator->GetPawn()))
    {
      Reward->ApplyExpeReward(Exp);
    }
  }
#pragma region Sound
  else 
  {
    // 데미지 입었을 때 사운드 재생
    if(HitSounds.Num() > 0)
    {
      int32 RandomIndex = FMath::RandRange(0, HitSounds.Num() - 1);
      UGameplayStatics::PlaySoundAtLocation(
        this, 
        HitSounds[RandomIndex], 
        GetActorLocation(),
        1.f,
        1.f,
        0.f,
        EnemyAttenuation
      );
    }
  }
#pragma endregion 최윤서
  return ActualDamage;
}



void ASanzoEnemyBase::Die()
{
  if (bIsDead) return;
  bIsDead = true;

#pragma region Sound
  // 사망 사운드 재생
  if(DeathSounds.Num() > 0)
  {
    int32 RandomIndex = FMath::RandRange(0, DeathSounds.Num() - 1);
    UGameplayStatics::PlaySoundAtLocation(
      this, 
      DeathSounds[RandomIndex], 
      GetActorLocation(),
      1.f,
      1.f,
      0.f,
      EnemyAttenuation
    );
  }
#pragma endregion 최윤서

#pragma region Call RoomBase
  // 사망 SanzoRoomBase::OnEnemyKilled() 호출
  if (CurrentRoom)
  {
    FVector Position = GetActorLocation();
    Position.Z -= 50.f; // 높이 조절
    CurrentRoom->OnEnemyKilled(Position);
  }
#pragma endregion 최윤서

  // AI 로직 정지
  AAIController* AICon = Cast<AAIController>(GetController());
  if (AICon && AICon->GetBrainComponent())
  {
    AICon->GetBrainComponent()->StopLogic(TEXT("Enemy Died"));
  }

#pragma region OverHeadUI
  //HPBar 숨기기
  if (OverHeadWidget)
  {
    OverHeadWidget->SetVisibility(false);
  }
  //TimerHandle 초기화
  GetWorldTimerManager().ClearTimer(OverHeadWidgetUpdateTimerHandle);

#pragma endregion 이준로
  // 충돌 끄기 및 래그돌(물리) 실행
  GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
  GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
  GetMesh()->SetSimulatePhysics(true);

  // 일정 시간 후 액터 제거
  SetLifeSpan(5.f);
}
#pragma endregion 김동주

void ASanzoEnemyBase::Attack()
{
  if (AttackMontage && !bIsDead)
  {
    PlayAnimMontage(AttackMontage);
  }
}

bool ASanzoEnemyBase::CanAttack(AActor* Target)
{
  if (!Target) return false;

  FHitResult Hit;
  FCollisionQueryParams Params;
  Params.AddIgnoredActor(this);

  // 가슴 높이에서 플레이어의 가슴 높이로 레이저 발사
  FVector Start = GetActorLocation() + FVector(0.f, 0.f, 50.f);
  FVector End = Target->GetActorLocation() + FVector(0.f, 0.f, 50.f);
  bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

  // 벽에 안 막혔거나
    // 코앞(250 이하)까지 붙어있다면 무조건 공격 시작
  if (!bHit
    || (Hit.GetActor() && Hit.GetActor()->ActorHasTag("Player"))
    || FVector::Distance(Start, End) <= 250.f)
  {
    return true;
  }

  // 벽에 가려져 있거나 타겟이 없다면 공격하지 않고 실패 반환
  return false;
}

#pragma region OverHeadUI

FEnemyOverHeadData ASanzoEnemyBase::MakeUpdateOverHeadData() const
{
  FEnemyOverHeadData NewData;
  if (MaxHP > 0.f)
  {
    NewData.HealthPercent = CurrentHP / MaxHP;
  }
  NewData.MaxStunCount = StunComponent ? StunComponent->GetMaxStunCount() : 0;
  
  NewData.CurrentStunCount = StunComponent ? StunComponent->GetCurrentStunCount() : 0;
	
	NewData.bIsStunned = StunComponent? StunComponent->GetIsStunned() : 0;
	
  NewData.bIsSighted = bIsSighted;

  return NewData;
}

void ASanzoEnemyBase::BroadCastAllData()
{
  OnEnemyDataChanged.Broadcast(MakeUpdateOverHeadData());
}

void ASanzoEnemyBase::MakeOverHeadWidget3D()
{
  if (!OverHeadWidget) return;

  APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
  if (!CameraManager) return;

  FVector CameraLocation = CameraManager->GetCameraLocation();
  FVector WidgetLocation = OverHeadWidget->GetComponentLocation();

  FRotator LookCameraRotation = UKismetMathLibrary::FindLookAtRotation(WidgetLocation, CameraLocation);
  OverHeadWidget->SetWorldRotation(LookCameraRotation);
}

void ASanzoEnemyBase::ShowAlertWidget(bool bIsSight)
{
  bIsSighted = bIsSight;
  
  BroadCastAllData();
}

#pragma endregion 이준로


#pragma endregion 김동주

#pragma region ProximitySensor
void ASanzoEnemyBase::OnProximityOverlap(
  UPrimitiveComponent* OverlappedComp,
  AActor* OtherActor,
  UPrimitiveComponent* OtherComp,
  int32 OtherBodyIndex,
  bool bFromSweep,
  const FHitResult& SweepResult)
{
  if (OtherActor && OtherActor->ActorHasTag("Player"))
  {
    if (ASanzoAIController* AICon = Cast<ASanzoAIController>(GetController()))
    {
      if (UBlackboardComponent* BBComp = AICon->GetBlackboardComponent())
      {
        if (BBComp->GetValueAsObject(TEXT("TargetActor")) == nullptr)
        {
          ShowAlertWidget(true);
          BBComp->SetValueAsObject(TEXT("TargetActor"), OtherActor);
          BBComp->ClearValue(TEXT("InvestigateLocation"));
        }
      }
    }
  }
}
#pragma endregion 김동주

#pragma region StunComponent
void ASanzoEnemyBase::OnStunCountChangedCallback(int32 CurrentStun, int32 MaxStun)
{
  BroadCastAllData();
}

void ASanzoEnemyBase::OnStunEnteredCallback()
{
  if (WeaponMesh) WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
  if (StaticWeaponMesh) StaticWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

  if (StunSound)
  {
    // 사운드 큐로 변경 - 최윤서
    UGameplayStatics::PlaySoundAtLocation(
      this, 
      StunSound, 
      GetActorLocation(),
      1.f,
      1.f,
      0.f,
      EnemyAttenuation
    );
  }

  StopAnimMontage();
  if (StunMontage)
  {
    PlayAnimMontage(StunMontage);
  }

  if (AAIController* AICon = Cast<AAIController>(GetController()))
  {
    if (UBehaviorTreeComponent* BTComp = Cast<UBehaviorTreeComponent>(AICon->GetBrainComponent()))
    {
      BTComp->StopTree(EBTStopMode::Safe);
    }
  }
  BroadCastAllData();
}

void ASanzoEnemyBase::OnStunRecoveredCallback()
{
  if (bIsDead) return;

  StopAnimMontage(StunMontage);

  if (AAIController* AICon = Cast<AAIController>(GetController()))
  {
    if (UBehaviorTreeComponent* BTComp = Cast<UBehaviorTreeComponent>(AICon->GetBrainComponent()))
    {
    	//Stun해제 방송
    	BroadCastAllData();
    	
      BTComp->RestartTree();
    }
  }
}

void ASanzoEnemyBase::OnParriedCallback()
{
  if (bIsDead) return;

  FVector EffectLocation = WeaponMesh ? WeaponMesh->GetComponentLocation() : GetActorLocation();
  
  if (ParriedEffect)
  {
    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParriedEffect, EffectLocation);
  }

  if (ParriedSound)
  {
    // 사운드 큐로 변경  - 최윤서
    UGameplayStatics::PlaySoundAtLocation(
      this, 
      ParriedSound, 
      GetActorLocation(),
      1.f,
      1.f,
      0.f,
      EnemyAttenuation
    );
  }

  if (!StunComponent->GetIsStunned() && StaggerMontage && !bIsEnraged)
  {
  	//Stun상태 방송
  	BroadCastAllData();

    PlayAnimMontage(StaggerMontage);
  }
}
#pragma endregion 김동주