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
#include "Chaos/Deformable/MuscleActivationConstraints.h"
#include "Dataflow/DataflowContent.h"
#include "UI/SanzoEnemyOverHeadWidget.h"

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

  // 적 무기 컴포넌트 설정
  WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
  // 오른손 뼈(hand_r)에 무기를 기본적으로 부착
  WeaponMesh->SetupAttachment(GetMesh(), TEXT("hand_r"));
  WeaponMesh->SetCollisionProfileName(TEXT("NoCollision"));

#pragma region OverHeadUI

  OverHeadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverHeadWidget"));

  OverHeadWidget->SetupAttachment(GetCapsuleComponent());

  OverHeadWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 115.0f));

  OverHeadWidget->SetRelativeScale3D(FVector(.3f, 0.3f, 0.3f));
	
	OverHeadWidget->SetDrawSize(FVector2D(500,900));

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

}

void ASanzoEnemyBase::BeginPlay()
{
  Super::BeginPlay();
  CurrentHP = MaxHP;
  bIsDead = false;

#pragma region OverHeadUI
	
	if (OverHeadWidget)
	{
		UUserWidget* OverHeadWidgetInstance = OverHeadWidget->GetUserWidgetObject();
		
		USanzoEnemyOverHeadWidget* CurrentWidget = Cast<USanzoEnemyOverHeadWidget>(OverHeadWidgetInstance);
		
		if (CurrentWidget)
		{
			OnEnemyDataChanged.AddDynamic(CurrentWidget, &USanzoEnemyOverHeadWidget::UpdateOverHeadWidget);
			
			BroadCastAllData();
		}
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

  if (GEngine)
  {
    FString Message = FString::Printf(TEXT("[Enemy Recieved] Damage: %.1f | HP Left: %.1f"),
      ActualDamage,
      CurrentHP);

    GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, Message);
  }

  UE_LOG(LogKDJ, Warning, TEXT("Enemy Took Damage: %f"), ActualDamage);

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
  }

  return ActualDamage;
}

void ASanzoEnemyBase::Die()
{
  if (bIsDead) return;
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

  // 들고 있던 무기도 물리 엔진을 켜서 바닥에 툭 떨어지게 만듦
  if (WeaponMesh)
  {
    WeaponMesh->SetCollisionProfileName(TEXT("Ragdoll"));
    WeaponMesh->SetSimulatePhysics(true);
    WeaponMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
  }

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

#pragma region OverHeadUI

FEnemyOverHeadData ASanzoEnemyBase::MakeUpdateOverHeadData() const
{
	FEnemyOverHeadData NewData;
	if (MaxHP > 0.f)
	{
		
		NewData.HealthPercent = CurrentHP / MaxHP;
		UE_LOG(LogLJR,Warning,TEXT("적 체력 퍼센트 : %f"),NewData.HealthPercent);
	}
	NewData.CurrentStunCount = StunCount;
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
	UE_LOG(LogLJR,Warning,TEXT("봤는가? %s"), bIsSighted ? TEXT("true") : TEXT("false"));
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