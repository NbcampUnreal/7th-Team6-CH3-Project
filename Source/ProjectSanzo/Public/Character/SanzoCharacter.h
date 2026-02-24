// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "GameplayTagContainer.h"
#include "Components/TimelineComponent.h"
#include "GameplayTagAssetInterface.h"
#include "Interface/SanzoTagEditorInterface.h"
#include "SanzoCharacter.generated.h" 

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class USanzoStatComponent;
class USanzoParryComponent;
class USanzoEquipmentComponent;
class USanzoNavigationArrowComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogSanzo, Log, All);

UCLASS(abstract)
class PROJECTSANZO_API ASanzoCharacter : 
  public ACharacter, 
  public IGameplayTagAssetInterface, 
  public ISanzoTagEditorInterface
  
{
  GENERATED_BODY()
#pragma region Component

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
  USpringArmComponent* CameraBoom;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
  UCameraComponent* FollowCamera;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
  UInputMappingContext* DefaultMappingContext;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
  USkeletalMeshComponent* TargetMesh;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Components", meta = (AllowPrivateAccess = "true"))
  USanzoStatComponent* StatComp;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Components", meta = (AllowPrivateAccess = "true"))
  USanzoParryComponent* ParryComp;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Components", meta = (AllowPrivateAccess = "true"))
  USanzoEquipmentComponent* EquipmentComp;

#pragma endregion 김형백

#pragma region NavigationComponent
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Navigation", meta = (AllowPrivateAccess = "true"))
	USanzoNavigationArrowComponent* NavArrow;
#pragma endregion 이준로


#pragma region InputActions
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
  UInputAction* MoveAction;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
  UInputAction* LookAction;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
  UInputAction* SprintAction;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
  UInputAction* DodgeAction;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
  UInputAction* FireAction;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
  UInputAction* AimAction;
  
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
  UInputAction* ParryAction;
#pragma endregion 김형백
public:
#pragma region Aiming

  FTimeline AimTimeline;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AimingTimeLine")
  UCurveFloat* AimCurve;

  UFUNCTION()
  void TimelineUpdateCallBack(float Value);
  UFUNCTION()
  void TimelineFinishedCallBack();
  UFUNCTION()
  void PlayAimTimeLine();
#pragma endregion 김형백
  
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Animation")
  UAnimMontage* DodgeMontage;

public:
  ASanzoCharacter();

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Movement")
  float NomalSpeed;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Movement")
  float SprintSpeedMultiplier;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Movement")
  float SprintSpeed;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Movement")
  float AimingSpeed;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Movement")
  float AimingSpeedMultiplier;  

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Tags")
  FGameplayTagContainer CharacterGameplayTags;

  //태그확인 델리케이트에 바인딩 하는 함수
  bool CheckTags(const FGameplayTag& TagsToCheck);

  //인터페이스 구현
  virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
  virtual void AddGameplayTag(FGameplayTag TagToAdd) override;
  virtual void RemoveGameplayTag(FGameplayTag TagToRemove) override;

protected:
#pragma region InputFunctions
  void Move(const FInputActionValue& Value);

  void Look(const FInputActionValue& Value);

  void SprintStart(const FInputActionValue& Value);

  void StopSprint(const FInputActionValue& Value);

  void FireStart(const FInputActionValue& Value);

  void StopFire(const FInputActionValue& Value);

  void Dodge(const FInputActionValue& Value);

  void Parry(const FInputActionValue& Value);
  void EndParry(UAnimMontage* Montage, bool bInterrupted);

  void AimStart(const FInputActionValue& Value);

  void AimStop(const FInputActionValue& Value);

  virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
#pragma endregion 김형백

#pragma region ActorLifecycle
  virtual void PostInitializeComponents() override;
  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;
  virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
#pragma endregion 김형백

  //타이머 핸들러 선언
  FTimerHandle SprintStaminaTimerHandle;
  FTimerHandle ParryPenaltyTimerHandle;

  void PrintGameplayTags();

public:
  FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
  FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

#pragma region PlayerTakeDamage
public:
  virtual float TakeDamage(
    float DamageAmount,
    struct FDamageEvent const& DamageEvent,
    class AController* EventInstigator,
    AActor* DamageCauser) override;
#pragma endregion 김동주



};
