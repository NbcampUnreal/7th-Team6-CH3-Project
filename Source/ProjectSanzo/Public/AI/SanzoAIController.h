#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "SanzoAIController.generated.h"

UCLASS()
class PROJECTSANZO_API ASanzoAIController : public AAIController
{
  GENERATED_BODY()

public:
  ASanzoAIController();

protected:
  virtual void OnPossess(APawn* InPawn) override;

#pragma region Perception
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
  TObjectPtr<class UAIPerceptionComponent> AIPerception;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
  TObjectPtr<class UAISenseConfig_Sight> SightConfig;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
  TObjectPtr<class UAISenseConfig_Hearing> HearingConfig;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
  TObjectPtr<class UAISenseConfig_Damage> DamageConfig;

  UFUNCTION()
  void OnTargetDetected(AActor* Actor, FAIStimulus Stimulus);
#pragma endregion

};
