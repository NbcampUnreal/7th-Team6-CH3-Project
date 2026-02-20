
#include "Character/Components/SanzoNavigationArrowComponent.h"

#include "Common/SanzoLog.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Stage/SanzoStageGate.h"
#include "Stage/SanzoStageManager.h"

USanzoNavigationArrowComponent::USanzoNavigationArrowComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	ArrowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArrowMesh"));
	ArrowMesh->SetupAttachment(this);
	
	ArrowMesh->SetHiddenInGame(true);
	ArrowMesh->SetCollisionProfileName(TEXT("NoCollision"));
}

void USanzoNavigationArrowComponent::ActivateNavigation()
{
	bIsActive = true;
	ArrowMesh->SetHiddenInGame(false);
}

void USanzoNavigationArrowComponent::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundGates;
	UGameplayStatics::GetAllActorsOfClass(
		GetWorld(),
		ASanzoStageGate::StaticClass(),
		FoundGates
	);
	
	if (FoundGates.Num() > 0)
	{
		TargetActor = Cast<ASanzoStageGate>(FoundGates[0]);
		UE_LOG(LogLJR, Warning, TEXT("NavArrow: 문을 찾았습니다"));
	}
	else
	{
		UE_LOG(LogCYS, Error, TEXT("NavArrow: 문을 찾지 못했습니다"));
	}
	
	AActor* StageManagerActor = UGameplayStatics::GetActorOfClass(GetWorld(), ASanzoStageManager::StaticClass());
	ASanzoStageManager* SanzoStageManager = Cast<ASanzoStageManager>(StageManagerActor);
	if (SanzoStageManager)
	{
		SanzoStageManager->StageCleared.AddDynamic(this, &USanzoNavigationArrowComponent::ActivateNavigation);
	}
}

void USanzoNavigationArrowComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (bIsActive && TargetActor)
	{
		//문에 대한 회전값 계산
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetComponentLocation(), TargetActor->GetActorLocation());
		
		//화상표 회전 업데이트
		SetWorldRotation(LookAtRotation);
		
		//위 아래로 둥둥 뜨는 효과
		float SineValue = FMath::Sin(GetWorld()->GetTimeSeconds() * 4.0f) * 15.0f;
		ArrowMesh->SetRelativeLocation(FVector(0.0f, 0.0f, SineValue));
		
	}
	
}

