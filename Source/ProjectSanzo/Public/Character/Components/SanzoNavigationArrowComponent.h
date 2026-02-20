// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "SanzoNavigationArrowComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTSANZO_API USanzoNavigationArrowComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USanzoNavigationArrowComponent();
	
	UFUNCTION()
	void ActivateNavigation();

protected:
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ArrowMesh;
	
	UPROPERTY(EditAnywhere)
	AActor* TargetActor;
	
	bool bIsActive = false;
		
};

