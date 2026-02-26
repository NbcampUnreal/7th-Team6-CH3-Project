#include "Items/SanzoItemBase.h"

#include "Character/Components/SanzoEquipmentComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

#pragma region ItemBase
ASanzoItemBase::ASanzoItemBase()
{
  PrimaryActorTick.bCanEverTick = true;
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	Collision->SetupAttachment(Scene);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(Collision);

	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComp"));
	NiagaraComp->SetupAttachment(RootComponent);

	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
	Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	// 이벤트 바인딩
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ASanzoItemBase::OnItemOverlap);
}

void ASanzoItemBase::BeginPlay()
{
  Super::BeginPlay();
	if (SpawnEffect)
	{
		NiagaraComp->SetAsset(SpawnEffect);
		NiagaraComp->Activate();
	}
}

void ASanzoItemBase::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);
	// 회전
  FRotator Rotation = GetActorRotation();
  Rotation.Yaw += DeltaTime * 45.f; // 초당 45도 회전
  SetActorRotation(Rotation);
}

void ASanzoItemBase::OnItemOverlap(
	UPrimitiveComponent* OverlappedComp, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, 
	bool bFromSweep, 
	const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{
		ActivateItem(OtherActor);
	}
}

void ASanzoItemBase::ActivateItem(AActor* Activator)
{
	UParticleSystemComponent* Particle = nullptr;
	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			PickupSound,
			GetActorLocation()
		);
	}
}

void ASanzoItemBase::DestroyItem()
{
	Destroy();
}
#pragma endregion 최윤서

#pragma region ItemPickedUpNotify

void ASanzoItemBase::NotifyItemPickedUp(AActor* Activator, int32 InAmount)
{
	if (Activator)
	{
		if (auto* EquipmentComponent = Activator->GetComponentByClass<USanzoEquipmentComponent>())
		{
			EquipmentComponent->NotifyItemPickedUp(ItemType, InAmount);
		}
	}
}

#pragma endregion 이준로