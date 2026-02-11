#include "Stage/SanzoStageGate.h"

ASanzoStageGate::ASanzoStageGate()
{
	PrimaryActorTick.bCanEverTick = false;
  Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
  SetRootComponent(Root);

  GateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GateMesh"));
  GateMesh->SetupAttachment(Root);

  TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
  TriggerBox->SetupAttachment(Root);

  TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
  TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
  TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ASanzoStageGate::OpenGate()
{
  if (bIsOpened) return;
  UE_LOG(LogTemp, Warning, TEXT("SG: 문 열림"));
  bIsOpened = true;
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
    UE_LOG(LogTemp, Warning, TEXT("SG: 플레이어 들어감"));

    // 여기서 StageManager에 다음 방 이동 요청
    OnGateEntered.Broadcast();
  }
}