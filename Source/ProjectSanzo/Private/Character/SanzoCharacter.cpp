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
#include "Character/Components/SanzoUpgradeComponent.h"
#include "Weapon/SanzoWeaponBase.h"
#include "Curves/CurveFloat.h"
#include "Common/SanzoGameplayTag.h"
#include "Common/SanzoLog.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Sound/AmbientSound.h"
#include "Core/SanzoGameInstance.h"
#include "Common/SanzoDamageType_Percent.h"
#include "Engine/DamageEvents.h"
#include "Core/SanzoBaseValue.h"

DEFINE_LOG_CATEGORY(LogSanzo);


#pragma region LifeCycle

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

  NormalSpeed = 500.f;
  SprintSpeedMultiplier = 1.8f;
  AimingSpeedMultiplier = 0.7f;
  SprintSpeed = NormalSpeed * SprintSpeedMultiplier; 
  AimingSpeed = NormalSpeed * AimingSpeedMultiplier;
  GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
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

  if(GetMesh())
  {
    GetMesh()->SetHiddenInGame(true);
    GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
  }

  TargetMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TargetMesh"));
  TargetMesh->SetupAttachment(GetMesh());

  FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
  FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
  FollowCamera->bUsePawnControlRotation = false;

  CameraSocketOffSet = FVector(0, 55, 68);
  CameraBoom->SocketOffset = CameraSocketOffSet;

  EquipmentComp = CreateDefaultSubobject<USanzoEquipmentComponent>(TEXT("Equipment"));
  StatComp = CreateDefaultSubobject<USanzoStatComponent>(TEXT("Stat"));
  ParryComp = CreateDefaultSubobject<USanzoParryComponent>(TEXT("Parry"));
  
#pragma endregion 김형백 
	
#pragma region UpgradeComponentInit
	
	UpgradeComp = CreateDefaultSubobject<USanzoUpgradeComponent>(TEXT("Upgrade"));
	
#pragma endregion 이준로
	
#pragma region NavigationComponentInit
  //컴포넌트 생성 및 부착

  NavArrow = CreateDefaultSubobject<USanzoNavigationArrowComponent>(TEXT("NavArrow"));
  NavArrow->SetupAttachment(RootComponent);

  //위치 조정
  NavArrow->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f));
  NavArrow->SetHiddenInGame(true);
#pragma endregion 이준로

  DodgeCooldownTime = 0.3;
  CurrentFOV = FollowCamera->FieldOfView;
  //틱켜키
  PrimaryActorTick.bCanEverTick = true;
}

void ASanzoCharacter::PostInitializeComponents()
{
  Super::PostInitializeComponents();
  //스탯 컴포넌트 델리게이 바인딩
  if (StatComp)
  {
    StatComp->TagCheckDelegate.BindUObject(this, &ASanzoCharacter::CheckTags);
    
  }
  //패리 몽타주 끝날때 델리게이트 바인딩
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

  //화살 장전 몽타주 시작 델리게이 바인딩
  if (EquipmentComp)
  {
    ASanzoWeaponBase* BowWeapon = EquipmentComp->Inventory[1];
    if (BowWeapon)
    {
      BowWeapon->StartBowAttackDelegate.BindUObject(this, &ASanzoCharacter::ZoomBow);
    }
  }
  
  // 스탯 복원 최
  RestoreFromGI();

  // BGM 액터 찾기
  TArray<AActor*> FoundActors;
  UGameplayStatics::GetAllActorsOfClass(
    GetWorld(),
    AAmbientSound::StaticClass(),
    FoundActors
  );

  if (FoundActors.Num() > 0)
  {
    BGMActor = Cast<AAmbientSound>(FoundActors[0]);
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

}

#pragma endregion 김형백

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
    EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started, this, &ASanzoCharacter::Pause);
    EnhancedInputComponent->BindAction(CheatKey, ETriggerEvent::Started, this, &ASanzoCharacter::Cheat);
    // 이용호 추가
    EnhancedInputComponent->BindAction(SwapAction, ETriggerEvent::Started, this, &ASanzoCharacter::SwapWeaponAction);
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
  if (CharacterGameplayTags.HasTag(SanzoTags::HitReaction))
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
  bool bIsHit = CharacterGameplayTags.HasTag(SanzoTags::HitReaction);
  
  
  if (bHasAimingTag || bHasAttackTag || !bShouldMove || bIsExhausted||bIsHit)
  {
    StopSprint(Value);
    return;
  }


  if (GetCharacterMovement() && bShouldMove)
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
    }
  }
}

void ASanzoCharacter::StopSprint(const FInputActionValue& Value)
{
  if(GetCharacterMovement())
  {
    GetCharacterMovement()->bOrientRotationToMovement = false;
    bUseControllerRotationYaw = true;
    GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

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
  if (CharacterGameplayTags.HasTag(SanzoTags::HitReaction))
  {
    StopFire(Value);
    return;
  }
  if (CharacterGameplayTags.HasTag(SanzoTags::Action_Fixed))
  {
    StopFire(Value);
    return;
  }
  if (CharacterGameplayTags.HasTag(SanzoTags::Swap))
  {
    return;
  }
  // dead 상태에서는 공격불가 - 최윤서
  if(CharacterGameplayTags.HasTag(SanzoTags::Dead))
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

      if (Weapon == EquipmentComp->Inventory[static_cast<uint8>(EWeaponType::Bow)])
      {

      }

      //AI가 들을수 있는 Noise재생
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
      //끼고있는 무기가 활 일때 공격불가
      if (Weapon == EquipmentComp->Inventory[1] && CharacterGameplayTags.HasTag(SanzoTags::Swap))
      {
        return;
      }

      Weapon->StopFire();
    }
  }
  GetWorldTimerManager().ClearTimer(BowDrawTimerHandle);
  ZoomOutBow();
}

void ASanzoCharacter::Pause(const FInputActionValue& Value)
{
  if (ASanzoPlayerController* PlayerController = Cast<ASanzoPlayerController>(GetController()))
  {
    PlayerController->ShowPopUp(SanzoTags::Pause);
  }

}
//회피
#pragma region Dodge
void ASanzoCharacter::Dodge(const FInputActionValue& Value)
{
  // dead 상태에서는 회피불가 - 최윤서
  if (CharacterGameplayTags.HasTag(SanzoTags::Dead))
  {
    return;
  }
  bool bIsExhausted = CharacterGameplayTags.HasTag(SanzoTags::Exhausted);
  if (CharacterGameplayTags.HasTag(SanzoTags::HitReaction))
  {
    return;
  }

  CharacterGameplayTags.AddTag(SanzoTags::Dodge);
  //실행불가
  if (bIsExhausted)
  {
    EndDodge(nullptr, false);
    return;
  }

  if (CharacterGameplayTags.HasTag(SanzoTags::Parry))
  {
    GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("뭔가이상함"));
    EndDodge(nullptr, false);
    return;
  }
  if (!TryDodge())
  {
    EndDodge(nullptr, false);
    return;
  }
  //총쏠때 회피누르면 정지
  if (CharacterGameplayTags.HasTag(SanzoTags::Attack))
  {
    StopFire(Value);
  }

  //회피실행
  if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
  {
    AnimInstance->Montage_Play(DodgeMontage);
    switch (DodgeIndex)
    {
    case 0:
      DodgeIndex++;
      break;
    case 1:
      AnimInstance->Montage_JumpToSection(FName("Dodge1"), DodgeMontage);
      DodgeIndex = 0;
      break;
    }
    
    FOnMontageBlendingOutStarted DodgeEndDelegate;
    DodgeEndDelegate.BindUObject(this, &ASanzoCharacter::EndDodge);
    AnimInstance->Montage_SetBlendingOutDelegate(DodgeEndDelegate, DodgeMontage);

    StatComp->ConsumeStaminaForAction(EActionType::Dodge);

  }
  
}

void ASanzoCharacter::EndDodge(UAnimMontage* Montage, bool bInterrupted)
{
  CharacterGameplayTags.RemoveTag(SanzoTags::Dodge);
}

bool ASanzoCharacter::TryDodge()
{
  if (GetWorld())
  {
    float CurrentTime = GetWorld()->GetTimeSeconds(); //회피 시도한 시간 기록
    float TimeSinceLastDodge = CurrentTime - LastDodgeTime;

    if (TimeSinceLastDodge < DodgeCooldownTime)
    {
      return false; //회피 쿨타임이 아직 남아있으면 패리 재생안함
    }
    LastDodgeTime = CurrentTime;
  }
  else
  {
    return false; //월드가 없으면 회피 시도 실패
  }
  return true; //회피 성공
}

void ASanzoCharacter::SuccessDodge()
{
  CharacterGameplayTags.RemoveTag(SanzoTags::Exhausted);
  UGameplayStatics::PlaySoundAtLocation(GetWorld(), DodgeSuccessSound, GetActorLocation());
  GetWorld()->GetWorldSettings()->SetTimeDilation(0.3f); //성공시 시간 느리게
  TWeakObjectPtr<ASanzoCharacter> WeakThis(this);
  GetWorld()->GetTimerManager().SetTimer(
    SlowTimerHandle,
    [WeakThis]()
    {
      if (WeakThis.IsValid())
        WeakThis->GetWorld()->GetWorldSettings()->SetTimeDilation(1.0f); //시간 정상화
    },
    0.1f, //0.1초 후 시간 원래대로
    false);
}
#pragma endregion 김형백

//패리
#pragma region Parry
void ASanzoCharacter::Parry(const FInputActionValue& Value)
{
  // dead 상태에서는 패링불가 - 최윤서
  if (CharacterGameplayTags.HasTag(SanzoTags::Dead))
  {
    return;
  }
  bool bIsExhausted = CharacterGameplayTags.HasTag(SanzoTags::Exhausted);

  if (CharacterGameplayTags.HasTag(SanzoTags::HitReaction))
  {
    return;
  }
  if (bIsExhausted)
  {
    EndParry(nullptr, false);
    return;
  }
  if (CharacterGameplayTags.HasTag(SanzoTags::Action) &&
    !CharacterGameplayTags.HasTag(SanzoTags::Aiming) &&
    !CharacterGameplayTags.HasTag(SanzoTags::Parry))
  {
    EndParry(nullptr, false);
    return;
  }

  if (ParryComp->TryParry())
  {
    ParryComp->PlayParryMontage();
    StatComp->ConsumeStaminaForAction(EActionType::Parry); //스태미나 소모
    StatComp->BeginExhaustionCooldown(); // 지친 상태로 가는 쿨다운 시작

    CharacterGameplayTags.AddTag(SanzoTags::Parry);
    CharacterGameplayTags.AddTag(SanzoTags::ParryPenaltyActive);

    ParryComp->ApplyParrySpamPenalty(); //패널티 체크 및 적용함수
    

    GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Parry!"));
  }
  else
  {
    GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Parry Failed!"));
  }

 
}

void ASanzoCharacter::EndParry(UAnimMontage* Montage, bool bInterrupted)
{
  //패리관련 태그 제거
    CharacterGameplayTags.RemoveTag(SanzoTags::Parry);
    CharacterGameplayTags.RemoveTag(SanzoTags::ParryWindow);
    
}
#pragma endregion 김형백

#pragma region AimingFunction
void ASanzoCharacter::AimStart(const FInputActionValue& Value)
{
  if(CharacterGameplayTags.HasTag(SanzoTags::Aiming))
  {
    return;
  }

  // dead 상태에서는 에임불가 - 최윤서
  if (CharacterGameplayTags.HasTag(SanzoTags::Dead))
  {
    return;
  }
  GetCharacterMovement()->MaxWalkSpeed = AimingSpeed;
  PlayAimTimeLine();
  CharacterGameplayTags.AddTag(SanzoTags::Aiming);
}

void ASanzoCharacter::AimStop(const FInputActionValue& Value)
{
  
  GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
  PlayAimTimeLine();
  CharacterGameplayTags.RemoveTag(SanzoTags::Aiming);
}

void ASanzoCharacter::ZoomBow(UAnimMontage* Montage)
{
  // dead 상태에서는 줌불가 - 최윤서
  if (CharacterGameplayTags.HasTag(SanzoTags::Dead))
  {
    return;
  }
  //float CurrentFOV = FollowCamera->FieldOfView;
  //float DesiredFOV = 45.f;
  // 활을 발사하는 사람 몽타지 중 Draw 섹션의 재생 길이를 가져옴
  int32 SectionIndex = Montage->GetSectionIndex(FName("Default"));
  float CurrentSectionLength = Montage->GetSectionLength(SectionIndex);

  float InterpSpeed = 1 / (CurrentSectionLength * 3);

  GetWorldTimerManager().ClearTimer(BowDrawTimerHandle);
  TWeakObjectPtr<ASanzoCharacter> WeakThis(this);
  GetWorldTimerManager().SetTimer(
    BowDrawTimerHandle,
    [WeakThis, InterpSpeed]()
    {
      float DesiredFOV = 75.f;
      if (WeakThis.IsValid())
      {
        WeakThis->CurrentFOV = WeakThis->FollowCamera->FieldOfView;
        float NewFOV = FMath::FInterpTo(WeakThis->CurrentFOV, DesiredFOV, WeakThis->GetWorld()->GetDeltaSeconds(), InterpSpeed);
        WeakThis->FollowCamera->SetFieldOfView(NewFOV);
      }
    },
    0.01f,
    true
  );
}

void ASanzoCharacter::ZoomOutBow()
{
  GetWorldTimerManager().ClearTimer(BowDrawTimerHandle);
  TWeakObjectPtr<ASanzoCharacter> WeakThis(this);
  float ZoomAlpha = 0;
  GetWorldTimerManager().SetTimer(
    BowDrawTimerHandle,
    [WeakThis, ZoomAlpha]() mutable
    {
      float DesiredFOV = 90.f;
      if (WeakThis.IsValid())
      {
        
        ZoomAlpha = FMath::Clamp(ZoomAlpha, 0, 1) + 0.05;
        WeakThis->CurrentFOV = WeakThis->FollowCamera->FieldOfView;
        float NewFOV = FMath::Lerp(WeakThis->CurrentFOV, 90.f, ZoomAlpha);
        WeakThis->FollowCamera->SetFieldOfView(NewFOV);

        if (ZoomAlpha >= 1.0f)
        {
          WeakThis->GetWorldTimerManager().ClearTimer(WeakThis->BowDrawTimerHandle);
        }
      }
    },
    0.01f,
    true
  );


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

#pragma region 스왑 액션 추가
void ASanzoCharacter::SwapWeaponAction(const FInputActionValue& Value)
{
  // 고정 액션, 공격, 조준, 스왑 중이면 안켜짐
  if (CharacterGameplayTags.HasTag(SanzoTags::Action_Fixed) ||
    CharacterGameplayTags.HasTag(SanzoTags::Attack)||
    CharacterGameplayTags.HasTag(SanzoTags::Swap))
  {
    return;
  }

  if (EquipmentComp)
  {
    // 이큅컴포넌트에서 몽타지 재생 시키고 몽타지 받아옴
    UAnimMontage* PlayedMontage = EquipmentComp->BeginSwapWeapon();

    if (PlayedMontage)
    {
      // 스왑 태그 붙이기
      CharacterGameplayTags.AddTag(SanzoTags::Swap);

      // 몽타지 종료 시 락을 풀기 위한 델리게이트 바인딩
      if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
      {
        FOnMontageEnded EndDelegate;
        EndDelegate.BindUObject(this, &ASanzoCharacter::EndWeaponSwap);
        AnimInstance->Montage_SetEndDelegate(EndDelegate, PlayedMontage);
      }
    }
  }
}
// 노티파이 들어왔을 때 실제로 무기 교체 처리할 함수
void ASanzoCharacter::ExecuteWeaponSwap()
{
  if (EquipmentComp)
  {
    // 무기 배열의 다음 인덱스 무기로 교체
    EquipmentComp->SwapWeapon(false);
  }
}
// 스왑 몽타지 끝날 떄 호출됨
void ASanzoCharacter::EndWeaponSwap(UAnimMontage* Montage, bool bInterrupted)
{
  // 태그 제거
  CharacterGameplayTags.RemoveTag(SanzoTags::Swap);
}

void ASanzoCharacter::ForceResetState()
{
  // 피격 태그 제거
  CharacterGameplayTags.RemoveTag(SanzoTags::HitReaction);
  // 스왑 태그 제거
  CharacterGameplayTags.RemoveTag(SanzoTags::Swap);
}
#pragma endregion 이용호

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

void ASanzoCharacter::ApplyUpgrade(EUpgradeTarget Target, EUpgradeType Type, float Value)
{
  if (Target == EUpgradeTarget::Character)
  {
    switch(Type)
    {
    case EUpgradeType::Beauty:
      ChangeModeling(Value);
      break;
    default:
      GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("샤갈! 이상한값이 발생했어요!"));
      break;
    }
  }
}

void ASanzoCharacter::ApplyExpeReward(float Amount)
{
  //겸치증가
  if(StatComp)
  {
    StatComp->AddExperience(Amount);
  }
}
#pragma endregion 김형백

#pragma region ChangeModeling
void ASanzoCharacter::ChangeModeling(float Value)
{
  FaceLevel += static_cast<uint8>(FMath::RoundToFloat(Value));

  switch (FaceLevel)
  {
  case 1: //로우폴리
    TargetMesh->SetSkeletalMeshAsset(LowPoly);
    TargetMesh->SetAnimInstanceClass(LowPolyABP);
    for( ASanzoWeaponBase* Weapon: EquipmentComp->Inventory)
    {
      Weapon->AttachToComponent(
        TargetMesh,
        FAttachmentTransformRules::SnapToTargetIncludingScale,
        Weapon->AttachSocketName
      );
    }
    //TODO :카메라 셋팅
    CameraSocketOffSet = FVector(0, 42, 53);
    CameraBoom->SocketOffset = CameraSocketOffSet;
    break;
  case 2: //아리사
    TargetMesh->SetSkeletalMeshAsset(Arisa);
    TargetMesh->SetAnimInstanceClass(ArisaABP);
    for (ASanzoWeaponBase* Weapon : EquipmentComp->Inventory)
    {
      Weapon->AttachToComponent(
        TargetMesh,
        FAttachmentTransformRules::SnapToTargetIncludingScale,
        Weapon->AttachSocketName
      );
      CameraSocketOffSet = FVector(0, 32, 41);
      CameraBoom->SocketOffset = CameraSocketOffSet;
    }
    break;
    //TODO :카메라 셋팅
  case 3: //래드돌
    TargetMesh->SetSkeletalMeshAsset(RadDoll);
    TargetMesh->SetAnimInstanceClass(RadDollABP);
    TargetMesh->SetRelativeScale3D(FVector(1.2, 1.2, 1.2));
    for (ASanzoWeaponBase* Weapon : EquipmentComp->Inventory)
    {
      Weapon->AttachToComponent(
        TargetMesh,
        FAttachmentTransformRules::SnapToTargetIncludingScale,
        Weapon->AttachSocketName
      );
      Weapon->GetWeaponMesh()->SetRelativeScale3D(FVector(0.7, 0.7, 0.7));
     // Weapon->GetWeaponMesh()->SetWorldScale3D
    }
    break;
    //TODO :카메라 셋팅
  }

}
#pragma endregion 김형백


#pragma region TakeDamage
float ASanzoCharacter::TakeDamage(
  float DamageAmount,
  FDamageEvent const& DamageEvent,
  AController* EventInstigator,
  AActor* DamageCauser)
{
  float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

#pragma region DamageTypePercent
  if (DamageEvent.DamageTypeClass == USanzoDamageType_Percent::StaticClass())
  {
    if (StatComp)
    {
      FinalDamage = StatComp->GetMaxHealth() * FinalDamage;
    }
  }
#pragma endregion 김동주

  /*패리 성공*/
  //패리태그가 있거나 성공섹션일때 패리 성공
  UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
  FName CurrentSection = AnimInstance->Montage_GetCurrentSection();
  if (CharacterGameplayTags.HasTag(SanzoTags::ParryWindow) || CurrentSection == FName("Success"))
  {
    ParryComp->SuccessParry();
    CharacterGameplayTags.RemoveTag(SanzoTags::Exhausted);
    FinalDamage = 0.f;
    
    //50%확률로 딜 반사 딜 반사 
    if (FMath::RandBool()) 
    {
      UGameplayStatics::ApplyDamage(DamageCauser, DamageAmount, GetController(), this, UDamageType::StaticClass());
    }

    GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, TEXT("Parried! No Damage Taken."));
    return FinalDamage;
  }

  /*회피성공*/
  if (CharacterGameplayTags.HasTag(SanzoTags::IFrame))
  {
    SuccessDodge();
    FinalDamage = 0.f;
    GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, TEXT("회피성공 ㅎㅎ"));
    return FinalDamage;
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
      //HandleDeath();
    }
  }
  ApplyHitEffect();

  return FinalDamage;
}

void ASanzoCharacter::ApplyHitEffect()
{
  if (CharacterGameplayTags.HasTag(SanzoTags::Parry))
  {
    return;
  }

  CharacterGameplayTags.AddTag(SanzoTags::HitReaction);
  if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
  {
    int32 RandomHitIndex = FMath::RandRange(0, HitMontage.Num() - 1);
    AnimInstance->Montage_Play(HitMontage[RandomHitIndex]); //여러개 작동 하게 나중에
    
    
    // Hit Sound - 최윤서
    if (HitSounds.Num() > 0)
    {
      int32 RandomIndex = FMath::RandRange(0, HitSounds.Num() - 1);
      UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSounds[RandomIndex], GetActorLocation());
    }

    /*FOnMontageEnded HitEndDelegate;*/
    FOnMontageBlendingOutStarted HitEndDelegate;
    HitEndDelegate.BindUObject(this, &ASanzoCharacter::EndHitEffect);

    // 반드시 Bind 후에 Set해야 함
    /*AnimInstance->Montage_SetEndDelegate(HitEndDelegate, HitMontage[RandomHitIndex]);*/
    AnimInstance->Montage_SetBlendingOutDelegate(HitEndDelegate, HitMontage[RandomHitIndex]);
    //공격중지
    StopFire(0);
  }
}

void ASanzoCharacter::EndHitEffect(UAnimMontage* Montage, bool bInterrupted)
{
  if (bInterrupted)
  {
    return;
  }
  CharacterGameplayTags.RemoveTag(SanzoTags::HitReaction);
}
#pragma endregion 김형백

// 자가 복원 최윤서
void ASanzoCharacter::RestoreFromGI()
{
  if (USanzoGameInstance* GI = GetGameInstance<USanzoGameInstance>())
  {
    GI->RestoreStat(this);
  }
}
#pragma region Death

void ASanzoCharacter::HandleDeath()
{
  // 중복 실행 방지
  if (CharacterGameplayTags.HasTag(SanzoTags::Dead))
  {
    return;
  }

  CharacterGameplayTags.AddTag(SanzoTags::Dead);

  // 이동 완전 정지
  GetCharacterMovement()->DisableMovement();

  // Capsule 비활성화
  GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

  // 공격/행동 태그 제거
  CharacterGameplayTags.RemoveTag(SanzoTags::Attack);
  CharacterGameplayTags.RemoveTag(SanzoTags::Sprint);
  CharacterGameplayTags.RemoveTag(SanzoTags::Aiming);
  CharacterGameplayTags.RemoveTag(SanzoTags::Dodge);
  CharacterGameplayTags.RemoveTag(SanzoTags::Parry);

  // 입력 비활성화
  GetController()->SetIgnoreMoveInput(true);
  GetController()->SetIgnoreLookInput(true);
  PlayDeathSequence();
}

void ASanzoCharacter::PlayDeathSequence()
{
  // 죽음 시퀀스 재생
  // BGM 정지
   if (BGMActor)
  {
    if (UAudioComponent* AudioComp = BGMActor->GetAudioComponent())
    {
      AudioComp->Stop();
    }
  }
  // Death Sound
  if(DeathSound)
  {
    UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSound, GetActorLocation());
  }
  // 슬로모션
  GetWorld()->GetWorldSettings()->SetTimeDilation(0.2f);

  // 레그돌
  GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
  GetMesh()->SetSimulatePhysics(true);
  GetMesh()->WakeAllRigidBodies();
	
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (ASanzoPlayerController* SanzoPlayerController = Cast<ASanzoPlayerController>(PlayerController))
		{
			SanzoPlayerController->ShowMainUI(FGameplayTag::RequestGameplayTag(FName("Game.State.GameOver")));
		}
	}
}


#pragma endregion 최윤서

#pragma region StatusDisplayData

TArray<FStatusDisplayData> ASanzoCharacter::GetStatusDisplayData() const
{
	TArray<FStatusDisplayData> DisplayData;
	
//캐릭터
	DisplayData.Add(FStatusDisplayData(
			EUpgradeTarget::Character,
			FUpgradeStatKey(EUpgradeTarget::Stat,EUpgradeType::MaxHealth),
			FText::FromString(TEXT("최대 체력")),
			FCharacterBaseValues::MaxHealth,
			StatComp->GetMaxHealth()
		)
	);
	DisplayData.Add(FStatusDisplayData(
		EUpgradeTarget::Character,
		FUpgradeStatKey(EUpgradeTarget::Stat,EUpgradeType::MaxStamina),
		FText::FromString(TEXT("최대 스태미나")),
		FCharacterBaseValues::MaxStamina,
		StatComp->GetMaxStamina()
	)
);
	
	DisplayData.Add(FStatusDisplayData(
		EUpgradeTarget::Character,
		FUpgradeStatKey(EUpgradeTarget::Character,EUpgradeType::Speed),
		FText::FromString(TEXT("이동 속도")),
		FCharacterBaseValues::DefaultMoveSpeed,
		NormalSpeed
	)
);
	
	DisplayData.Add(FStatusDisplayData(
	EUpgradeTarget::Character,
	FUpgradeStatKey(EUpgradeTarget::Character,EUpgradeType::Beauty),
	FText::FromString(TEXT("링크 강화")),
	FCharacterBaseValues::DefaultFaceLevel,
	FaceLevel
	)
);
	
//외모관련 수치 필요 (동기화 단계)
	
	//총
	DisplayData.Add(FStatusDisplayData(
		EUpgradeTarget::Gun,
		FUpgradeStatKey(EUpgradeTarget::Gun,EUpgradeType::Damage),
		FText::FromString(TEXT("공격력")),
		FGunBaseValues::BaseDamage,
		EquipmentComp->GetGunDamage()
	)
);
	DisplayData.Add(FStatusDisplayData(
		EUpgradeTarget::Gun,
		FUpgradeStatKey(EUpgradeTarget::Gun,EUpgradeType::FireRate),
		FText::FromString(TEXT("발사 속도")),
		FGunBaseValues::BaseFireRate,
		EquipmentComp->GetGunFireRate()
	)
);
	
	//활
	DisplayData.Add(FStatusDisplayData(
		EUpgradeTarget::Bow,
		FUpgradeStatKey(EUpgradeTarget::Bow,EUpgradeType::Damage),
		FText::FromString(TEXT("공격력")),
		FBowBaseValues::BaseDamage,
		EquipmentComp->GetBowDamage()
	)
);
	DisplayData.Add(FStatusDisplayData(
		EUpgradeTarget::Bow,
		FUpgradeStatKey(EUpgradeTarget::Bow,EUpgradeType::MaxChargeTime),
		FText::FromString(TEXT("차징 시간")),
		FBowBaseValues::BaseMaxChargeTime,
		EquipmentComp->GetBowChargeTime()
	)
);
	
	
	return DisplayData;
}

#pragma endregion 이준로


void ASanzoCharacter::Cheat()
{
  StatComp->AddExperience(100);
}
