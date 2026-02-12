// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/SanzoCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Character/Components/SanzoStatComponent.h"
#include "Character/Components/SanzoParryComponent.h"
#include "Character/Components/SanzoEquipmentComponent.h"
#include "Weapon/SanzoWeaponBase.h"
#include "Weapon/SanzoGun.h"

DEFINE_LOG_CATEGORY(LogSanzo);

ASanzoCharacter::ASanzoCharacter()
{
  
#pragma region MovementInit 
  bUseControllerRotationPitch = false;
  bUseControllerRotationYaw = true;
  bUseControllerRotationRoll = false;

  GetCharacterMovement()->bOrientRotationToMovement = true;
  GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
  GetCharacterMovement()->JumpZVelocity = 700.f;
  GetCharacterMovement()->AirControl = 0.35f;

  NomalSpeed = 500.f;
  SprintSpeedMultiplier = 1.8f;
  SprintSpeed = NomalSpeed * SprintSpeedMultiplier; 

  GetCharacterMovement()->MaxWalkSpeed = NomalSpeed;
  //게임패드 아날로그 스틱 최소이동속도
  GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;

  //감속힘
  GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
  GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
#pragma endregion 김형백 
#pragma region ComponentInit
  GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
  CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
  CameraBoom->SetupAttachment(RootComponent);
  CameraBoom->TargetArmLength = 400.0f;
  CameraBoom->bUsePawnControlRotation = true;

  FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
  FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
  FollowCamera->bUsePawnControlRotation = false;

  EquipmentComp = CreateDefaultSubobject<USanzoEquipmentComponent>(TEXT("Equipment"));
  StatComp = CreateDefaultSubobject<USanzoStatComponent>(TEXT("Stat"));
  ParryComp = CreateDefaultSubobject<USanzoParryComponent>(TEXT("Parry"));
  
#pragma endregion 김형백 

  
}

void ASanzoCharacter::BeginPlay()
{

  Super::BeginPlay();

  if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
  {
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
    {
      Subsystem->AddMappingContext(DefaultMappingContext, 0);
    }
  }

}

#pragma region InputFunction
void ASanzoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
  if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

    //EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
    //EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
    EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASanzoCharacter::Move);

    EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASanzoCharacter::Look);
    EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ASanzoCharacter::SprintStart);
    EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ASanzoCharacter::StopSprint); 
    EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ASanzoCharacter::FireStart);
    EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ASanzoCharacter::StopFire);
  }
  else
  {
    UE_LOG(LogSanzo, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
  }
}

void ASanzoCharacter::Move(const FInputActionValue& Value)
{
  FVector2D MovementVector = Value.Get<FVector2D>();

  if (Controller != nullptr)
  {
    const FRotator Rotation = Controller->GetControlRotation();
    const FRotator YawRotation(0, Rotation.Yaw, 0);

    const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

    const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    AddMovementInput(ForwardDirection, MovementVector.Y);
    AddMovementInput(RightDirection, MovementVector.X);
  }
}

void ASanzoCharacter::Look(const FInputActionValue& Value)
{
  FVector2D LookAxisVector = Value.Get<FVector2D>();

  if (Controller != nullptr)
  {
    AddControllerYawInput(LookAxisVector.X);
    AddControllerPitchInput(LookAxisVector.Y);
  }
}

void ASanzoCharacter::SprintStart(const FInputActionValue& Value)
{

  if(GetCharacterMovement())
  {
    GetCharacterMovement()->bOrientRotationToMovement = true;
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
    //TODO : 스태미너 감소 시작
   
    
    GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red,
      FString::Printf(TEXT("현재속도 : %.1f, 현재 스태미너 : %.1f"), GetCharacterMovement()->MaxWalkSpeed, StatComp->GetStamina()));
  }
}

void ASanzoCharacter::StopSprint(const FInputActionValue& Value)
{
  if(GetCharacterMovement())
  {
    GetCharacterMovement()->bOrientRotationToMovement = false;
    bUseControllerRotationYaw = true;
    GetCharacterMovement()->MaxWalkSpeed = NomalSpeed;
    //TODO : 스태미너 감소 중지
    GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red,
      FString::Printf(TEXT("현재속도 : %.1f, 현재 스태미너 : %.1f"), GetCharacterMovement()->MaxWalkSpeed, StatComp->GetStamina()));
  }
}

void ASanzoCharacter::FireStart(const FInputActionValue& Value)
{
  
  if(EquipmentComp)
  {
    GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Component extists"));
    if (ASanzoWeaponBase* Weapon = EquipmentComp->CurrentWeapon)
    {
      GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Current weapon exists"));
      Weapon->StartFire();
      
    }
  }
}

void ASanzoCharacter::StopFire(const FInputActionValue& Value)
{
  if (EquipmentComp)
  {
    
    if (ASanzoWeaponBase* Weapon = EquipmentComp->CurrentWeapon)
    {
      
      Weapon->StopFire();

    }
  }
}

void ASanzoCharacter::Dodge(const FInputActionValue& Value)
{
  
}
#pragma endregion 김형백