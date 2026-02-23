#include "Stage/SanzoStageGate.h"
#include "Common/SanzoLog.h"
#include "Kismet/GameplayStatics.h"

#pragma region Stage Gate

ASanzoStageGate::ASanzoStageGate()
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

  TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
  TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
  TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ASanzoStageGate::OpenGate()
{
  if (bIsOpened) return;
  UE_LOG(LogCYS, Warning, TEXT("SG: 문 열림"));
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
    &ASanzoStageGate::RotateDoor,
    0.016f,
    true
  );
}

void ASanzoStageGate::BeginPlay()
{
  Super::BeginPlay();
  TriggerBox->OnComponentBeginOverlap.AddDynamic(
    this,
    &ASanzoStageGate::OnOverlapBegin
  );
}

void ASanzoStageGate::OnOverlapBegin(
  UPrimitiveComponent* OverlappedComponent,
  AActor* OtherActor,
  UPrimitiveComponent* OtherComp,
  int32 OtherBodyIndex,
  bool bFromSweep,
  const FHitResult& SweepResult)
{
  if (!bIsOpened) return;

  if (OtherActor && OtherActor->ActorHasTag("Player"))
  {
    UE_LOG(LogCYS, Warning, TEXT("SG: 플레이어 들어감"));

    // 여기서 StageManager에 다음 방 이동 요청
    OnGateEntered.Broadcast();
  }
}

void ASanzoStageGate::RotateDoor()
{
  float DeltaTime = GetWorld()->GetDeltaSeconds();
  float DeltaYaw = RotateSpeed * DeltaTime;

  GateMeshL->AddRelativeRotation(FRotator(0.f, -DeltaYaw, 0.f));
  GateMeshR->AddRelativeRotation(FRotator(0.f, DeltaYaw, 0.f));

  RotatedAmount += DeltaYaw;

  if (RotatedAmount >= TargetAngle)
  {
    GetWorldTimerManager().ClearTimer(DoorTimer);
  }
}

#pragma endregion 최윤서