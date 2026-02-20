// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/SanzoCharacter.h"
#include "Character/SanzoPlayerController.h"
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
#include "Character/Components/SanzoNavigationArrowComponent.h"
#include "Weapon/SanzoWeaponBase.h"
#include "Weapon/SanzoGun.h"
#include "Curves/CurveFloat.h"

#include "Common/SanzoLog.h"

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
  AimingSpeedMultiplier = 0.7f;
  SprintSpeed = NomalSpeed * SprintSpeedMultiplier; 
  AimingSpeed = NomalSpeed * AimingSpeedMultiplier;
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

  TargetMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TargetMesh"));
  TargetMesh->SetupAttachment(GetMesh());

  FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
  FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
  FollowCamera->bUsePawnControlRotation = false;

  EquipmentComp = CreateDefaultSubobject<USanzoEquipmentComponent>(TEXT("Equipment"));
  StatComp = CreateDefaultSubobject<USanzoStatComponent>(TEXT("Stat"));
  ParryComp = CreateDefaultSubobject<USanzoParryComponent>(TEXT("Parry"));
  
#pragma endregion 김형백 
#pragma region NavigationComponentInit
	//컴포넌트 생성 및 부착
	if (NavigationArrow)
	{
		NavigationArrow = CreateDefaultSubobject<USanzoNavigationArrowComponent>(TEXT("NavigationArrow"));
		NavigationArrow->SetupAttachment(RootComponent);
	
		//위치 조정
		NavigationArrow->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f));
	}
	
#pragma endregion 이준로

  //태그캐싱
  AimingTag = FGameplayTag::RequestGameplayTag(FName("Character.Action.Aiming"));
  SprintTag = FGameplayTag::RequestGameplayTag(FName("Character.Action.Sprint"));
  AttackTag = FGameplayTag::RequestGameplayTag(FName("Character.Action.Attack"));
  ExhaustedTag = FGameplayTag::RequestGameplayTag(FName("Character.Status.Exhausted"));
  //틱켜키
  PrimaryActorTick.bCanEverTick = true;
}

void ASanzoCharacter::PostInitializeComponents()
{
  Super::PostInitializeComponents();
  //태그확인용 델리게이 바인딩
  StatComp->TagCheckDelegate.BindUObject(this, &ASanzoCharacter::CheckTags);
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

  if (AimCurve)
  {
    //델리게이트 바인딩
    FOnTimelineFloat TimelineCallback;
    TimelineCallback.BindUFunction(this, FName("TimelineUpdateCallBack"));
    FOnTimelineEvent TimelineFinishedCallback;
    TimelineFinishedCallback.BindUFunction(this, FName("TimelineFinishedCallBack"));

    //타임라인과 커브 연결
    AimTimeline.AddInterpFloat(AimCurve, TimelineCallback);
    AimTimeline.SetTimelineFinishedFunc(TimelineFinishedCallback);

  }
}

void ASanzoCharacter::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

  if (AimTimeline.IsPlaying())
  {
    AimTimeline.TickTimeline(DeltaTime);
  }
  
  //확인용 지울예정
  GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red,
    FString::Printf(TEXT("현재속도 : %.1f, 현재 스태미너 : %.1f"), 
      GetCharacterMovement()->MaxWalkSpeed, StatComp->GetStamina()));
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
    EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &ASanzoCharacter::Dodge);
    EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ASanzoCharacter::AimStart);
    EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ASanzoCharacter::AimStop);
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
  bool bShouldMove = !(GetCharacterMovement()->GetCurrentAcceleration().IsNearlyZero());
  bool bHasAttackTag = CharacterGameplayTags.HasTag(AttackTag);   
  bool bHasAimingTag = CharacterGameplayTags.HasTag(AimingTag); 
  bool bIsExhausted = CharacterGameplayTags.HasTag(ExhaustedTag);
  
  if(bHasAimingTag||bHasAttackTag||!bShouldMove||bIsExhausted)
  {
    StopSprint(Value);
    return;
  }


  if(GetCharacterMovement() && bShouldMove)
  {
    GetCharacterMovement()->bOrientRotationToMovement = true;
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;

    //달리기 태그 추가
    CharacterGameplayTags.AddTag(SprintTag);
    //스태미나소모
    if (StatComp)
    {
      StatComp->RequestConsumeStaminaForSprint(true);
    }
  }
  
}

void ASanzoCharacter::StopSprint(const FInputActionValue& Value)
{
  if(GetCharacterMovement())
  {
    GetCharacterMovement()->bOrientRotationToMovement = false;
    bUseControllerRotationYaw = true;
    GetCharacterMovement()->MaxWalkSpeed = NomalSpeed;

    if (CharacterGameplayTags.HasTag(AimingTag))
    {
      GetCharacterMovement()->MaxWalkSpeed = AimingSpeed;
    }
    if (StatComp)
    {
      StatComp->RequestConsumeStaminaForSprint(false);
    }
    CharacterGameplayTags.RemoveTag(SprintTag);

  }
}

void ASanzoCharacter::FireStart(const FInputActionValue& Value)
{
  //if (CharacterGameplayTags.HasTag(SprintTag))
  //{
  //  StopFire(Value);
  //  return;
  //} 이제 달릴때 쏘면 멈추고 쏩니다

  if(EquipmentComp)
  {
    CharacterGameplayTags.AddTag(AttackTag);

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
    CharacterGameplayTags.RemoveTag(AttackTag);
    if (ASanzoWeaponBase* Weapon = EquipmentComp->CurrentWeapon)
    {
      Weapon->StopFire();
    }
  }
}

void ASanzoCharacter::Dodge(const FInputActionValue& Value)
{
  
}
#pragma region AimingFunction
void ASanzoCharacter::AimStart(const FInputActionValue& Value)
{
  
  if(CharacterGameplayTags.HasTag(AimingTag))
  {
    return;
  }
  GetCharacterMovement()->MaxWalkSpeed = AimingSpeed;
  PlayAimTimeLine();
  CharacterGameplayTags.AddTag(AimingTag);
}

void ASanzoCharacter::AimStop(const FInputActionValue& Value)
{
  
  GetCharacterMovement()->MaxWalkSpeed = NomalSpeed;
  PlayAimTimeLine();
  CharacterGameplayTags.RemoveTag(AimingTag);
}

void ASanzoCharacter::TimelineUpdateCallBack(float Value)
{
  CameraBoom->TargetArmLength = Value;
  
}

void ASanzoCharacter::TimelineFinishedCallBack()
{
}

void ASanzoCharacter::PlayAimTimeLine()
{
  if (AimCurve && !CharacterGameplayTags.HasTag(AimingTag))
  {
    AimTimeline.Play(); 
  }
  if (AimCurve && CharacterGameplayTags.HasTag(AimingTag))
  {
    AimTimeline.Reverse();
  }
}
#pragma endregion 김형백

#pragma endregion 김형백


bool ASanzoCharacter::CheckTags(const FGameplayTag& TagsToCheck)
{
  return CharacterGameplayTags.HasTag(TagsToCheck);
  
}

#pragma region PlayerTakeDamage
float ASanzoCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
  float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

  if (StatComp)
  {
    StatComp->ApplyDamage(FinalDamage);
    if (GEngine)
    {
      FString Msg = FString::Printf(TEXT("Player Hit! Damage: %.1f"), FinalDamage);
      GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, Msg);
    }

    if (StatComp->IsDead())
    {
      UE_LOG(LogKDJ, Error, TEXT("Player Died!"));
      // TO-DO: 플레이어 래그돌, 게임 오버 UI 호출, 조작 불가 등 처리
    }
  }

  return FinalDamage;
}
#pragma endregion 김동주
