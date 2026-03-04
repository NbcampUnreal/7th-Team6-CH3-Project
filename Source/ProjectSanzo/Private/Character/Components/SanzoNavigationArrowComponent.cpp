
#include "Character/Components/SanzoNavigationArrowComponent.h"

#include "Common/SanzoLog.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Stage/SanzoStageGate.h"
#include "Stage/SanzoStageManager.h"

USanzoNavigationArrowComponent::USanzoNavigationArrowComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
  
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
	}
	
	AActor* StageManagerActor = UGameplayStatics::GetActorOfClass(GetWorld(), ASanzoStageManager::StaticClass());
	ASanzoStageManager* SanzoStageManager = Cast<ASanzoStageManager>(StageManagerActor);
	if (SanzoStageManager)
	{
		SanzoStageManager->StageCleared.AddDynamic(this, &USanzoNavigationArrowComponent::ActivateNavigation);
	}
}

void USanzoNavigationArrowComponent::OnRegister()
{
	Super::OnRegister();
  // 1. 생성자에서 하위 메쉬 컴포넌트 생성 (이게 가장 안전합니다)
  ArrowMesh = NewObject<UStaticMeshComponent>(this,TEXT("ArrowMeshInstance"));
  ArrowMesh->SetupAttachment(this); // SceneComponent인 본체에 부착
  ArrowMesh->RegisterComponent();
 
  if (ArrowMesh && NavigationMeshAsset)
  {
    ArrowMesh->SetStaticMesh(NavigationMeshAsset);
  }	
  
  ArrowMesh->SetHiddenInGame(true);
  ArrowMesh->SetCastShadow(false);
  ArrowMesh->SetCollisionProfileName(TEXT("NoCollision"));
  
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
		float SineValue = FMath::Sin(GetWorld()->GetTimeSeconds() * 4.0f) * 10.0f;
		
		//Scale 값 변화에 따른 진폭 보정
		FVector ParentScale = GetComponentScale();
		
		float AdjustedX = (ParentScale.X != 0.0f) ? (SineValue / ParentScale.X) : SineValue;
		
		ArrowMesh->SetRelativeLocation(FVector(AdjustedX, 0.0f, 0.0f));
		
	}
	
}

