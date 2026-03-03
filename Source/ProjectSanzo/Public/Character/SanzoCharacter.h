// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "GameplayTagContainer.h"
#include "Components/TimelineComponent.h"
#include "GameplayTagAssetInterface.h"
#include "Interface/SanzoTagEditorInterface.h"
#include "Interface/SanzoUpgradeInterface.h"
#include "Interface/SanzoRewardReceiverInterface.h"
#include "Interface/SanzoCharacterInterface.h"
#include "SanzoCharacter.generated.h" 

class USanzoUpgradeComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class USanzoStatComponent;
class USanzoParryComponent;
class USanzoEquipmentComponent;
class USanzoNavigationArrowComponent;
class AAmbientSound;
class UNiagaraSystem;
class UNiagaraComponent;
DECLARE_LOG_CATEGORY_EXTERN(LogSanzo, Log, All);

UCLASS(abstract)
class PROJECTSANZO_API ASanzoCharacter : 
  public ACharacter, 
  public IGameplayTagAssetInterface, 
  public ISanzoTagEditorInterface,
  public ISanzoUpgradeInterface,
  public ISanzoRewardReceiverInterface,
  public ISanzoCharacterInterface


{
  GENERATED_BODY()
#pragma region Component

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
  USpringArmComponent* CameraBoom;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
  FVector CameraSocketOffSet;

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

#pragma region UpgradeComponent
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Components", meta = (AllowPrivateAccess = "true"))
	USanzoUpgradeComponent* UpgradeComp;
	
#pragma endregion 이준로
	
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

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
  UInputAction* PauseAction;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
  UInputAction* CheatKey;

  void Cheat();

  // 클리어 치트키 추가(0키로 지정) - 최윤서
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
  UInputAction* CheatClearKey;
 
  void CheatClear();
#pragma endregion 김형백

#pragma region 스왑 액션 추가
  // 스왑용 액션(임시로 Q키 지정)
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
  UInputAction* SwapAction;
#pragma endregion 이용호
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
  
#pragma region Asset
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Animation")
  UAnimMontage* DodgeMontage;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Dodge")
  USoundBase* DodgeSuccessSound;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Animation")
  TArray<UAnimMontage*> HitMontage;


#pragma endregion 김형백

#pragma region FaceUpgrade
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Transform")
  USkeletalMesh* LowPoly;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Transform")
  TSubclassOf<UAnimInstance> LowPolyABP;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Transform")
  USkeletalMesh* Arisa;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Transform")
  TSubclassOf<UAnimInstance> ArisaABP;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Transform")
  USkeletalMesh* RadDoll;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Transform")
  TSubclassOf<UAnimInstance> RadDollABP;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Transform")
  USoundBase* TransformationSound;

  // 에디터에서 할당할 나이아가라 에셋
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Transform")
  UNiagaraSystem* TransformationEffect;

  // 실제 캐릭터에 붙어서 실행될 컴포넌트
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Transform")
  UNiagaraComponent* TransformationNiagaraComponent;


  uint8 FaceLevel = 0;

  UFUNCTION()
  void ChangeModeling(float Value);


#pragma endregion 김형백



public:
  ASanzoCharacter();

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Movement")
  float NormalSpeed;
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
  virtual void ApplyUpgrade(EUpgradeTarget Target, EUpgradeType Type, float Value) override;
  virtual void ApplyExpeReward(float Amount) override; //다른 보상이 있다면 추가가능
  virtual ACharacter* GetCharacterActor() override { return this; }

protected:
#pragma region InputFunctions
  void Move(const FInputActionValue& Value);

  void Look(const FInputActionValue& Value);

  void SprintStart(const FInputActionValue& Value);

  void StopSprint(const FInputActionValue& Value);

  void FireStart(const FInputActionValue& Value);

  void StopFire(const FInputActionValue& Value);

  void Pause(const FInputActionValue& Value);

  void Dodge(const FInputActionValue& Value);
  void EndDodge(UAnimMontage* Montage, bool bInterrupted);
  bool TryDodge();
  void SuccessDodge();
  uint8 DodgeIndex = 0;
  float LastDodgeTime = 0;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Dodge")
  float DodgeCooldownTime;

  void Parry(const FInputActionValue& Value);
  void EndParry(UAnimMontage* Montage, bool bInterrupted);

  void AimStart(const FInputActionValue& Value);

  void AimStop(const FInputActionValue& Value);

  void ZoomBow(UAnimMontage* Montage);
  void ZoomOutBow();

  virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
#pragma endregion 김형백

#pragma region ActorLifecycle
  virtual void PostInitializeComponents() override;
  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;
  virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
#pragma endregion 김형백

  //현재 FOV
  float CurrentFOV;

  //타이머 핸들러 선언
  FTimerHandle SprintStaminaTimerHandle;
  FTimerHandle ParryPenaltyTimerHandle;
  FTimerHandle SlowTimerHandle;
  FTimerHandle BowDrawTimerHandle;
  
  //디버그용
  void PrintGameplayTags();

  void EndHitEffect(UAnimMontage* Montage, bool bInterrupted);


public:
  void ApplyHitEffect();
  FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
  FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

#pragma region 스왑 액션 추가
  // 스왑용 액션(임시로 Q키 지정)
  void SwapWeaponAction(const FInputActionValue& Value);

  // 애니메이션 노티파이에서 호출될 실제 스왑 실행 함수
  UFUNCTION(BlueprintCallable, Category = "Equipment")
  void ExecuteWeaponSwap();

  // 몽타주 종료 시 호출될 델리게이트 함수
  void EndWeaponSwap(UAnimMontage* Montage, bool bInterrupted);

  // 스왑 도중에 스왑 몽타지 끊겼을 때 태그 제거용 호출 함수
  void ForceResetState();
#pragma endregion 이용호

#pragma region PlayerTakeDamage
public:
  virtual float TakeDamage(
    float DamageAmount,
    struct FDamageEvent const& DamageEvent,
    class AController* EventInstigator,
    AActor* DamageCauser) override;
#pragma endregion 김동주

  // 스테이지 이동 시 GI에서 스탯 복원하는 함수
  void RestoreFromGI();

#pragma region Death
  void HandleDeath();
  void PlayDeathSequence();
#pragma endregion 최윤서

#pragma region Sounds
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Sounds")
  USoundBase* DeathSound;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Sounds")
  TArray<USoundBase*> HitSounds;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
  AAmbientSound* BGMActor;
#pragma endregion 최윤서
	
#pragma region StatusDisplayData
	
	TArray<FStatusDisplayData> GetStatusDisplayData() const;
	
#pragma endregion 이준로

};
