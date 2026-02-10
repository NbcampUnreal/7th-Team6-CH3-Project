// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "GameplayTagContainer.h"
#include "SanzoCharacter.generated.h"


class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class USanzoStatComponent;
class USanzoParryComponent;
class USanzoEquipmentComponent;
DECLARE_LOG_CATEGORY_EXTERN(LogSanzo, Log, All);

UCLASS(abstract)
class PROJECTSANZO_API ASanzoCharacter : public ACharacter
{
  GENERATED_BODY()
#pragma region Component

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
  USpringArmComponent* CameraBoom;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
  UCameraComponent* FollowCamera;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
  UInputMappingContext* DefaultMappingContext;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Components", meta = (AllowPrivateAccess = "true"))
  USanzoStatComponent* StatComp;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Components", meta = (AllowPrivateAccess = "true"))
  USanzoParryComponent* ParryComp;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Components", meta = (AllowPrivateAccess = "true"))
  USanzoEquipmentComponent* EquipmentComp;

#pragma endregion

#pragma region InputActions
 /* UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
  UInputAction* JumpAction;*/

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

#pragma endregion

public:
  ASanzoCharacter();

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Movement")
  float NomalSpeed;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Movement")
  float SprintSpeedMultiplier;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Movement")
  float SprintSpeed;

  FGameplayTagContainer GameplayTagContainer;
protected:

  void Move(const FInputActionValue& Value);

  void Look(const FInputActionValue& Value);

  void SprintStart(const FInputActionValue& Value);

  void StopSprint(const FInputActionValue& Value);

  void FireStart(const FInputActionValue& Value);

  void StopFire(const FInputActionValue& Value);


  virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

  virtual void BeginPlay();

public:
  FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
  FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
