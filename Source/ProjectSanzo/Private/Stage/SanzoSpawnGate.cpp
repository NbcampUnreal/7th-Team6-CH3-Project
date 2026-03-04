#include "Stage/SanzoSpawnGate.h"
#include "Common/SanzoLog.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"

#define COLLISION_GATEBLOCKER ECC_GameTraceChannel1

ASanzoSpawnGate::ASanzoSpawnGate()
{
  PrimaryActorTick.bCanEverTick = false;
  Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
  SetRootComponent(Root);

  GateMeshL = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GateMeshL"));
  GateMeshL->SetupAttachment(Root);

  GateMeshR = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GateMeshR"));
  GateMeshR->SetupAttachment(Root);

  TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
  TriggerBox->SetupAttachment(Root);

  // 트리거박스 타입 전용 채널로 설정
  TriggerBox->SetCollisionObjectType(COLLISION_GATEBLOCKER);

  // 모든 채널을 무시하도록 초기화 (슈팅 판정 등 영향 X)
  TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
  TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);

  // 플레이어(Pawn) Block
  TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
  // 적(PhysicsBody) Overlap
  TriggerBox->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
  TriggerBox->SetGenerateOverlapEvents(true);
}

void ASanzoSpawnGate::BeginPlay()
{
	Super::BeginPlay();
  LeftRot = GateMeshL->GetRelativeRotation();
  RightRot = GateMeshR->GetRelativeRotation();
	if (TriggerBox)
	{
		TriggerBox->OnComponentBeginOverlap.RemoveDynamic(this, &ASanzoSpawnGate::OnOverlapBegin);
		TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ASanzoSpawnGate::OnOverlapBegin);
		
		TriggerBox->OnComponentEndOverlap.RemoveDynamic(this, &ASanzoSpawnGate::OnOverlapEnd);
		TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ASanzoSpawnGate::OnOverlapEnd);
	}
}

void ASanzoSpawnGate::OpenGate()
{
  if (bIsOpened) return;
  ////UE_LOG(LogCYS, Warning, TEXT("SpawnGate: 문 열림"));
  bIsOpened = true;

  if (OpenSound)
  {
    UGameplayStatics::PlaySoundAtLocation(
      GetWorld(),
      OpenSound,
      GetActorLocation(),
      1.f,
      1.f,
      0.f,
      OpenSoundAttenuation
    );
  }

  RotatedAmount = 0.f;

  GetWorldTimerManager().SetTimer(
    DoorTimer,
    this,
    &ASanzoSpawnGate::RotateDoor,
    0.016f,
    true
  );
}
void ASanzoSpawnGate::CloseGate()
{
  if (!bIsOpened) return;
  ////UE_LOG(LogCYS, Warning, TEXT("SpawnGate: 문 닫힘"));
  bIsOpened = false;

  if (CloseSound)
  {
    UGameplayStatics::PlaySoundAtLocation(
      GetWorld(),
      CloseSound,
      GetActorLocation(),
      1.f,
      1.f,
      0.f,
      OpenSoundAttenuation
    );
  }

  RotatedAmount = 0.f;

  GetWorldTimerManager().SetTimer(
    DoorTimer,
    this,
    &ASanzoSpawnGate::RotateDoor,
    0.016f,
    true
  );
}
void ASanzoSpawnGate::RotateDoor()
{
  float DeltaTime = GetWorld()->GetDeltaSeconds();
  float DeltaYaw = RotateSpeed * DeltaTime;

  if (bIsOpened)
  {
    // 열릴 때
    GateMeshL->AddRelativeRotation(FRotator(0.f, DeltaYaw, 0.f));
    GateMeshR->AddRelativeRotation(FRotator(0.f, -DeltaYaw, 0.f));

    RotatedAmount += DeltaYaw;

    if (RotatedAmount >= TargetAngle)
    {
      GetWorldTimerManager().ClearTimer(DoorTimer);
    }
  }
  else
  {
    // 닫힐 때
    GateMeshL->AddRelativeRotation(FRotator(0.f, -DeltaYaw, 0.f));
    GateMeshR->AddRelativeRotation(FRotator(0.f, DeltaYaw, 0.f));

    RotatedAmount += DeltaYaw;

    if (RotatedAmount >= TargetAngle)
    {
      GetWorldTimerManager().ClearTimer(DoorTimer);
      GateMeshL->SetRelativeRotation(FRotator(LeftRot));
      GateMeshR->SetRelativeRotation(FRotator(RightRot));
    }
  }
}

void ASanzoSpawnGate::OnOverlapEnd(
  UPrimitiveComponent* OverlappedComponent,
  AActor* OtherActor,
  UPrimitiveComponent* OtherComp,
  int32 OtherBodyIndex)
{
  if (OtherActor && OtherActor->ActorHasTag("Enemy"))
  {
    //UE_LOG(LogCYS, Error, TEXT("SpawnGate: Enemy 나감"));
    CloseGate();
  }
}

void ASanzoSpawnGate::OnOverlapBegin(
  UPrimitiveComponent* OverlappedComponent, 
  AActor* OtherActor, 
  UPrimitiveComponent* OtherComp, 
  int32 OtherBodyIndex, 
  bool bFromSweep, 
  const FHitResult& SweepResult)
{
  if (OtherActor && OtherActor->ActorHasTag("Enemy"))
  {
    OpenGate();
  }
}