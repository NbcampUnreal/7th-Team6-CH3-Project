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
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h" // 디버그 라인 그리기용
#include "Engine/DamageEvents.h"

DEFINE_LOG_CATEGORY(LogSanzo);

ASanzoCharacter::ASanzoCharacter()
{
  GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

  bUseControllerRotationPitch = false;
  bUseControllerRotationYaw = false;
  bUseControllerRotationRoll = false;

  GetCharacterMovement()->bOrientRotationToMovement = true;
  GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

  GetCharacterMovement()->JumpZVelocity = 700.f;
  GetCharacterMovement()->AirControl = 0.35f;
  GetCharacterMovement()->MaxWalkSpeed = 500.f;
  GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
  GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
  GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

  CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
  CameraBoom->SetupAttachment(RootComponent);
  CameraBoom->TargetArmLength = 400.0f;
  CameraBoom->bUsePawnControlRotation = true;

  FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
  FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
  FollowCamera->bUsePawnControlRotation = false;

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

void ASanzoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
  Super::SetupPlayerInputComponent(PlayerInputComponent);

  if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

    EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
    EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
    EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASanzoCharacter::Move);

    EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASanzoCharacter::Look);
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

#pragma region AttackTest

void ASanzoCharacter::TestLineTraceAttack()
{
  if (!FollowCamera) return;

  // [카메라 기준] 시선 방향으로 트레이스 (조준점 계산)
  FVector CameraStart = FollowCamera->GetComponentLocation();
  FVector CameraForward = FollowCamera->GetForwardVector();
  FVector CameraEnd = CameraStart + (CameraForward * TestAttackRange);

  // 트레이스 파라미터 설정
  FHitResult CameraHitResult;
  FCollisionQueryParams Params;
  Params.AddIgnoredActor(this); // 자기 자신은 무시

  // 라인 트레이스 발사
  bool bHit = GetWorld()->LineTraceSingleByChannel
  (
    CameraHitResult,
    CameraStart,
    CameraEnd,
    ECC_Visibility,
    Params
  );

  // 맞았으면 그 지점(ImpactPoint)
  // 안 맞았으면 허공의 끝점(CameraEnd)
  FVector TargetLocation = bHit ?
    CameraHitResult.ImpactPoint : CameraEnd;

  // [발사 원점] 캐릭터의 오른손에서 시작
  FVector MuzzleLocation = GetMesh()->GetSocketLocation(TEXT("hand_r"));

  // 트레이스 궤적 그리기
  // 맞았으면 빨간색, 아니면 초록색 선을 1초간 표시
  FColor LineColor = bHit ? FColor::Red : FColor::Green;
  DrawDebugLine
  (
    GetWorld(),
    MuzzleLocation,
    TargetLocation,
    LineColor,
    false,
    1.f,
    0,
    1.f
  );

  if (bHit && CameraHitResult.GetActor())
  {
    // 맞은 지점에 점 찍기
    DrawDebugPoint
    (
      GetWorld(),
      TargetLocation,
      10.f,
      FColor::Yellow,
      false,
      1.f
    );

    // 적에게 데미지 전달
    UGameplayStatics::ApplyPointDamage
    (
      CameraHitResult.GetActor(),
      TestBaseDamage,
      CameraForward,    // 공격 방향은 카메라 보는 방향
      CameraHitResult,
      GetController(),  // 공격자 컨트롤러
      this,             // 데미지 원인 액터
      UDamageType::StaticClass()
    );
  }
}
#pragma endregion

