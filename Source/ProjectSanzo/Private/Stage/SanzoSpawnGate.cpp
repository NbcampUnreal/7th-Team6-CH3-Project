#include "Stage/SanzoSpawnGate.h"
#include "Common/SanzoLog.h"
#include "Kismet/GameplayStatics.h"

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

  TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
  TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
  TriggerBox->SetGenerateOverlapEvents(true);

  // 플레이어 막기
  TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

  // Enemy(PhysicsBody) 감지
  TriggerBox->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
}

void ASanzoSpawnGate::BeginPlay()
{
	Super::BeginPlay();
  TriggerBox->OnComponentEndOverlap.AddDynamic(
    this,
    &ASanzoSpawnGate::OnOverlapEnd
  );
}
void ASanzoSpawnGate::OpenGate()
{
  if (bIsOpened) return;
  UE_LOG(LogCYS, Warning, TEXT("SpawnGate: 문 열림"));
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
  UE_LOG(LogCYS, Warning, TEXT("SpawnGate: 문 닫힘"));
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
    GateMeshL->AddRelativeRotation(FRotator(0.f, -DeltaYaw, 0.f));
    GateMeshR->AddRelativeRotation(FRotator(0.f, DeltaYaw, 0.f));

    RotatedAmount += DeltaYaw;

    if (RotatedAmount >= TargetAngle)
    {
      GetWorldTimerManager().ClearTimer(DoorTimer);
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
    UE_LOG(LogTemp, Warning, TEXT("SpawnGate:Enemy 나감"));
    CloseGate();
  }
}