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

#include "Common/SanzoGameplayTag.h"
#include "Common/SanzoLog.h"
#include "Components/PawnNoiseEmitterComponent.h"

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

  NavArrow = CreateDefaultSubobject<USanzoNavigationArrowComponent>(TEXT("NavArrow"));
  NavArrow->SetupAttachment(RootComponent);

  //위치 조정
  NavArrow->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f));
  NavArrow->SetHiddenInGame(true);
#pragma endregion 이준로

  //틱켜키
  PrimaryActorTick.bCanEverTick = true;
}

void ASanzoCharacter::PostInitializeComponents()
{
  Super::PostInitializeComponents();
  //태그확인용 델리게이 바인딩
  if (StatComp)
  {
    StatComp->TagCheckDelegate.BindUObject(this, &ASanzoCharacter::CheckTags);
  }

  //몽타주 끝날때 델리게이트 바인딩
  if(ParryComp)
  {
    ParryComp->BlendingOutDelegate.BindUObject(this, &ASanzoCharacter::EndParry);
  }
}

void ASanzoCharacter::BeginPlay()
{
  Super::BeginPlay();

  //인풋 시스템 설정
  if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
  {
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
    {
      Subsystem->AddMappingContext(DefaultMappingContext, 0);
    }
  }

  //타임라인 관련 설정
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
  PrintGameplayTags(); //겜태그확인용
  GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::Green,
    FString::Printf(TEXT("현재속도 : %.1f, 현재 스태미너 : %.1f"), 
      GetCharacterMovement()->MaxWalkSpeed, StatComp->GetStamina()));
}

void ASanzoCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
  Super::EndPlay(EndPlayReason);

  //스테이지 이동시 타이머 제거
  if(GetWorld()->GetTimerManager().IsTimerActive(SprintStaminaTimerHandle))
  {
    GetWorld()->GetTimerManager().ClearTimer(SprintStaminaTimerHandle);
  }
  if(GetWorld()->GetTimerManager().IsTimerActive(ExhaustionRecoveryTimerHandle))
  {
    GetWorld()->GetTimerManager().ClearTimer(ExhaustionRecoveryTimerHandle);
  }
}

void ASanzoCharacter::ExhaustionRecovery()
{

  CharacterGameplayTags.RemoveTag(SanzoTags::Exhausted);
  if (StatComp)
  {
    StatComp->bIsExhausted = false;
  }

}

void ASanzoCharacter::PrintGameplayTags()
{
  // 엔진이 유효한지 확인
  if (!GEngine) return;

  // 1. 태그가 하나도 없을 때의 처리
  if (CharacterGameplayTags.IsEmpty())
  {
    // Key 값을 100으로 고정하여 한 줄에서만 갱신되도록 설정
    GEngine->AddOnScreenDebugMessage(100, 0.0f, FColor::Red, TEXT("보유 중인 태그 없음"));
    return;
  }

  // 2. 컨테이너 내부의 태그들을 배열로 추출
  TArray<FGameplayTag> TagArray;
  CharacterGameplayTags.GetGameplayTagArray(TagArray);

  // 3. 배열을 순회하며 화면에 출력
  for (int32 i = 0; i < TagArray.Num(); ++i)
  {
    // 각 태그마다 고유한 Key(101, 102...)를 부여하여 줄이 겹치지 않게 함
    int32 Key = 101 + i;
    FString TagName = TagArray[i].ToString();
    FString Message = FString::Printf(TEXT("Tag [%d]: %s"), i, *TagName);

    // 시간을 0.0f로 설정하면 Tick에서 호출될 때 잔상 없이 매 프레임 깔끔하게 출력됨
    GEngine->AddOnScreenDebugMessage(Key, 0.0f, FColor::Magenta, Message);
  }
}

#pragma region InputFunction

void ASanzoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
  if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

    EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASanzoCharacter::Move);
    EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASanzoCharacter::Look);
    EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ASanzoCharacter::SprintStart);
    EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ASanzoCharacter::StopSprint); 
    EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ASanzoCharacter::FireStart);
    EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ASanzoCharacter::StopFire);
    EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &ASanzoCharacter::Dodge);
    EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ASanzoCharacter::AimStart);
    EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ASanzoCharacter::AimStop);
    EnhancedInputComponent->BindAction(ParryAction, ETriggerEvent::Started, this, &ASanzoCharacter::Parry);
  }
  else
  {
    UE_LOG(LogSanzo, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
  }
}

void ASanzoCharacter::Move(const FInputActionValue& Value)
{
  if(CharacterGameplayTags.HasTag(SanzoTags::Action_Fixed))
  {
    return;
  }

  FVector2D MovementVector = Value.Get<FVector2D>();

  if (Controller != nullptr)//움직임 로직
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
  bool bHasAttackTag = CharacterGameplayTags.HasTag(SanzoTags::Attack);   
  bool bHasAimingTag = CharacterGameplayTags.HasTag(SanzoTags::Aiming); 
  bool bIsExhausted = CharacterGameplayTags.HasTag(SanzoTags::Exhausted);
  
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
    CharacterGameplayTags.AddTag(SanzoTags::Sprint);
    //스태미나소모
    if (StatComp)
    {
      StatComp->RequestConsumeStaminaForSprint(true); // 스태미나 소모 호출
      if (StatComp->bIsExhausted)
      {
        CharacterGameplayTags.AddTag(SanzoTags::Exhausted); //여기 내부는 사실 컴포넌트에 있어야하는데 귀찮아서 여기 만듬 ㅎ;
        GetWorld()->GetTimerManager().SetTimer(
          ExhaustionRecoveryTimerHandle,
          this,
          &ASanzoCharacter::ExhaustionRecovery,
          3.f,
          false);
        StopSprint(Value);
      }
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

    if (CharacterGameplayTags.HasTag(SanzoTags::Aiming))
    {
      GetCharacterMovement()->MaxWalkSpeed = AimingSpeed;
    }
    if (StatComp)
    {
      StatComp->RequestConsumeStaminaForSprint(false);
    }
    CharacterGameplayTags.RemoveTag(SanzoTags::Sprint);

  }
}

void ASanzoCharacter::FireStart(const FInputActionValue& Value)
{
  //if (CharacterGameplayTags.HasTag(SprintTag))
  //{
  //  StopFire(Value);
  //  return;
  //} 이제 달릴때 쏘면 멈추고, 쏩니다
  if (CharacterGameplayTags.HasTag(SanzoTags::Action_Fixed))
  {
    return;
  }

  if(EquipmentComp)
  {
    CharacterGameplayTags.AddTag(SanzoTags::Attack);

    GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Component extists"));
    if (ASanzoWeaponBase* Weapon = EquipmentComp->CurrentWeapon)
    {
      GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Current weapon exists"));
      Weapon->StartFire();
#pragma region MakeNoise
      MakeNoise(1.0f, this, GetActorLocation());
#pragma endregion 김동주

    }
  }



}

void ASanzoCharacter::StopFire(const FInputActionValue& Value)
{
  if (EquipmentComp)
  {
    CharacterGameplayTags.RemoveTag(SanzoTags::Attack);
    if (ASanzoWeaponBase* Weapon = EquipmentComp->CurrentWeapon)
    {
      Weapon->StopFire();
    }
  }
}

void ASanzoCharacter::Dodge(const FInputActionValue& Value)
{

  
}

void ASanzoCharacter::Parry(const FInputActionValue& Value)
{
  CharacterGameplayTags.AddTag(SanzoTags::Parry);
  ParryComp->PlayParryMontage();
  GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Parry!"));
 

 
}

void ASanzoCharacter::EndParry(UAnimMontage* Montage, bool bInterrupted)
{
    CharacterGameplayTags.RemoveTag(SanzoTags::Parry);
}

#pragma region AimingFunction
void ASanzoCharacter::AimStart(const FInputActionValue& Value)
{
  
  if(CharacterGameplayTags.HasTag(SanzoTags::Aiming))
  {
    return;
  }
  GetCharacterMovement()->MaxWalkSpeed = AimingSpeed;
  PlayAimTimeLine();
  CharacterGameplayTags.AddTag(SanzoTags::Aiming);
}

void ASanzoCharacter::AimStop(const FInputActionValue& Value)
{
  
  GetCharacterMovement()->MaxWalkSpeed = NomalSpeed;
  PlayAimTimeLine();
  CharacterGameplayTags.RemoveTag(SanzoTags::Aiming);
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
  if (AimCurve && !CharacterGameplayTags.HasTag(SanzoTags::Aiming))
  {
    AimTimeline.Play(); 
  }
  if (AimCurve && CharacterGameplayTags.HasTag(SanzoTags::Aiming))
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
#pragma region InterfaceFunction
void ASanzoCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
  TagContainer = CharacterGameplayTags;
}

void ASanzoCharacter::AddGameplayTag(FGameplayTag TagToAdd)
{
  CharacterGameplayTags.AddTag(TagToAdd);
}

void ASanzoCharacter::RemoveGameplayTag(FGameplayTag TagToRemove)
{
  CharacterGameplayTags.RemoveTag(TagToRemove);
}
#pragma endregion 김형백

float ASanzoCharacter::TakeDamage(
  float DamageAmount,
  FDamageEvent const& DamageEvent,
  AController* EventInstigator,
  AActor* DamageCauser)
{
  float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
  if (CharacterGameplayTags.HasTag(SanzoTags::ParryWindow))
  {
    ParryComp->SuccessParry();
    FinalDamage = 0.f;
    GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, TEXT("Parried! No Damage Taken."));
  }

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

/*
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
*/