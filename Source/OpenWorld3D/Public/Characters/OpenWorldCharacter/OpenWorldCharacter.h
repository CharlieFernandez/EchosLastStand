// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/GameCharacter.h"
#include "InputActionValue.h"
#include "Items/Item.h"
#include "Items/Weapons/EquipActionState.h"

#include "OpenWorldCharacter.generated.h"

class UTransformComponent;
class ULockOnComponent;
class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class AItem;
class UAnimMontage;
class UCharacterMovementComponent;
class AWeapon;
class UAnimInstance;
class UOpenWorldCharacterHUD;

UCLASS(BlueprintType)
class OPENWORLD3D_API AOpenWorldCharacter : public AGameCharacter, public IPickUpInterface
{
   GENERATED_BODY()

public:    
   /* Methods */
   AOpenWorldCharacter();
   void Tick(float DeltaSeconds) override;
   virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
   FORCEINLINE static FName GetPlayerTag() { return PlayerTag; }
   virtual void SetOverlappingItem(AItem* Item) override { OverlappedItem = Item; }
   virtual void AddSouls(ASouls* Souls) override;
   virtual void AddGold(ATreasure* Treasure) override;
   
protected:
   /* Methods */
   void ObtainOrEquip();
   virtual void Jump() override;
   void TurnToSpiritForm();
   void TurnToHumanForm();
   bool HasMovementInput() const;
   virtual void BeginPlay() override;
   void RegenerateStamina(float DeltaTime) const;
   void UnlockIfTargetIsDead() const;
   void SetCombatTargetToLockedOnEnemy();
   void MoveInput(const FInputActionValue& Value);
   void LookAround(const FInputActionValue& Value);
   void RollInput();
   void TransformationInput();
   void FlyUp(const FInputActionValue& Value);
   void FlyDown(const FInputActionValue& Value);
   virtual void GetHit_Implementation(const FVector ImpactPoint, const FVector InstigatorPosition) override;

   
   /* Properties */   
   UPROPERTY(EditDefaultsOnly, Category = Input) TObjectPtr<UInputMappingContext> OpenWorldCharacterContext;
   UPROPERTY(EditDefaultsOnly, Category = Input) TObjectPtr<UInputAction> MovementAction;
   UPROPERTY(EditDefaultsOnly, Category = Input) TObjectPtr<UInputAction> LookAroundAction;
   UPROPERTY(EditDefaultsOnly, Category = Input) TObjectPtr<UInputAction> JumpAction;
   UPROPERTY(EditDefaultsOnly, Category = Input) TObjectPtr<UInputAction> EKeyPressedAction;
   UPROPERTY(EditDefaultsOnly, Category = Input) TObjectPtr<UInputAction> AttackPressedAction;
   UPROPERTY(EditDefaultsOnly, Category = Input) TObjectPtr<UInputAction> RollPressedAction;
   UPROPERTY(EditDefaultsOnly, Category = Input) TObjectPtr<UInputAction> TransformPressedAction;
   UPROPERTY(EditDefaultsOnly, Category = Input) TObjectPtr<UInputAction> LockOnAction;
   UPROPERTY(EditDefaultsOnly, Category = Input) TObjectPtr<UInputAction> LockOffAction;
   UPROPERTY(EditDefaultsOnly, Category = Input) TObjectPtr<UInputAction> FlyUpAction;
   UPROPERTY(EditDefaultsOnly, Category = Input) TObjectPtr<UInputAction> FlyDownAction;

   /* Functions */
   UFUNCTION()
   void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
   UFUNCTION()
   void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
   void IfNotAttackingThenMove();
   void IfAttackingThenChangeFacingDirection();

private:
   /* Fields */
   inline static const FName PlayerTag = FName("Player");
   
   TObjectPtr<UOpenWorldCharacterHUD> OpenWorldCharacterHUD;
   FVector2D CurrentMovementInput;
   FVector2D LastNonZeroMovementInput;
   bool IsSpiritForm;

   /* Methods */
   void BindHealthRadiusSphereComponents();
   void InitializeHUD();
   void EquipOrUnequipWeapon();
   void PlayEquipMontage(EEquipActionState EquipType) const;
   void PickUpWeapon(AWeapon* Weapon, UMeshComponent* WeaponMesh, FName SN);
   void LockOn();
   void LockOff();
   void ToggleAllMeshVisibility(bool IsVisible) const;
   void SetCollisionsForBody() const;
   void SetCollisionsForSpirit() const;
   void FaceTheLastMoveInput();
   
   /* Properties */
   
   TObjectPtr<APlayerController> PlayerController;
   
   UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
   TObjectPtr<UTransformComponent> TransformComponent;
   
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
   TObjectPtr<USkeletalMeshComponent> HairMeshComponent;
   
   UPROPERTY(EditDefaultsOnly, Category = Camera)
   float DefaultCameraDistance;

   UPROPERTY(EditDefaultsOnly, Category = Camera)
   float LowestCameraPositionRelativeToPlayer;

   UPROPERTY(EditDefaultsOnly, Category = Camera)
   float HighestCameraPositionRelativeToPlayer;
   
   UPROPERTY(EditDefaultsOnly)
   bool ShouldDrawHammerDownDebugTrace;
   
   UPROPERTY(VisibleAnywhere)
   TObjectPtr<ULockOnComponent> LockOnComponent;
   
   UPROPERTY(VisibleAnywhere)
   USphereComponent* HealthRadiusSphereComponent;
   
   UPROPERTY(VisibleAnywhere)
   TObjectPtr<USpringArmComponent> SpringArm;

   UPROPERTY(VisibleAnywhere)
   TObjectPtr<UCameraComponent> CameraBoom;
   
   UPROPERTY(VisibleInstanceOnly)
   TObjectPtr<AItem> OverlappedItem;

   UPROPERTY(EditDefaultsOnly, Category = Montages)
   TObjectPtr<UAnimMontage> EquipMontage;

   UPROPERTY(EditDefaultsOnly, Category = Camera)
   float CameraRotationSpeed;

   UPROPERTY(EditDefaultsOnly, Category = Stamina)
   float TransformationStaminaCost;
   
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Particles, meta = (AllowPrivateAccess = "true"))
   TObjectPtr<UNiagaraSystem> HammerDownParticles;

   UPROPERTY(VisibleDefaultsOnly)
   TObjectPtr<USceneComponent> TransformationVFXContainer;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Particles, meta = (AllowPrivateAccess = "true"))
   TObjectPtr<UNiagaraComponent> BodyToSpiritVFXComponent;
   
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Particles, meta = (AllowPrivateAccess = "true"))
   TObjectPtr<UNiagaraComponent> SpiritVFXComponent;
   
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Particles, meta = (AllowPrivateAccess = "true"))
   TObjectPtr<UNiagaraComponent> SpiritToBodyVFXComponent;

   /* Functions */
   UFUNCTION(BlueprintCallable)
   void SpawnHammerDownParticles();
};