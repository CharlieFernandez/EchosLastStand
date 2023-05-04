// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/GameCharacter.h"
#include "InputActionValue.h"
#include "Items/Item.h"
#include "Items/Weapons/EquipActionState.h"

#include "OpenWorldCharacter.generated.h"

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
	virtual void ResetActionState() override;
	void ObtainOrEquip();
	virtual void Jump() override;
	bool HasMovementInput() const;
	virtual void BeginPlay() override;
	void MoveInput(const FInputActionValue& Value);
	void DashInput(const FInputActionValue& Value);
	void LookAround(const FInputActionValue& Value);
	virtual void GetHit_Implementation(const FVector ImpactPoint, const FVector InstigatorPosition) override;

	
	/* Properties */	
	UPROPERTY(EditDefaultsOnly, Category = Input) TObjectPtr<UInputMappingContext> OpenWorldCharacterContext;	
	UPROPERTY(EditDefaultsOnly, Category = Input) TObjectPtr<UInputAction> MovementAction;
	UPROPERTY(EditDefaultsOnly, Category = Input) TObjectPtr<UInputAction> LookAroundAction;
	UPROPERTY(EditDefaultsOnly, Category = Input) TObjectPtr<UInputAction> JumpAction;
	UPROPERTY(EditDefaultsOnly, Category = Input) TObjectPtr<UInputAction> EKeyPressedAction;
	UPROPERTY(EditDefaultsOnly, Category = Input) TObjectPtr<UInputAction> AttackPressedAction;
	UPROPERTY(EditDefaultsOnly, Category = Input) TObjectPtr<UInputAction> DashAction;
	UPROPERTY(EditDefaultsOnly, Category = Input) TObjectPtr<UInputAction> LockOnAction;
	UPROPERTY(EditDefaultsOnly, Category = Input) TObjectPtr<UInputAction> LockOffAction;

	/* Functions */
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	void Move(FVector2D MovementVector);

private:
	/* Fields */
	inline static const FName PlayerTag = FName("Player");
	TObjectPtr<UOpenWorldCharacterHUD> OpenWorldCharacterHUD;
	float CurrentDashTime = 0;
	FVector2D LastNonZeroMovementInput;

	/* Methods */
	void BindHealthRadiusSphereComponents();
	void InitializeHUD(const APlayerController* PlayerController);
	void EquipOrUnequipWeapon();
	void PlayEquipMontage(EEquipActionState EquipType) const;
	void PickUpWeapon(AWeapon* Weapon, UMeshComponent* WeaponMesh, FName SN);
	void LockOn();
	void LockOff();
	void ToggleAllMeshVisibility(bool IsVisible) const;
	
	/* Properties */

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
	
	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<ULockOnComponent> LockOnComponent;
	
	UPROPERTY(VisibleAnywhere)
	USphereComponent* HealthRadiusSphereComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> CameraBoom;

	UPROPERTY(EditDefaultsOnly)
	float DashDurationInSeconds;
	
	UPROPERTY(EditDefaultsOnly)
	float DashSpeed;
	
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<AItem> OverlappedItem;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> EquipMontage;

	UPROPERTY(EditDefaultsOnly, Category = Camera)
	float CameraRotationSpeed;

	UPROPERTY(EditDefaultsOnly, Category = Stamina)
	float DashStaminaCost;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Particles, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UNiagaraSystem> HammerDownParticles;
	
	UPROPERTY(EditDefaultsOnly, Category = Particles)
	TObjectPtr<UNiagaraComponent> DashNiagaraComponent;

	/* Functions */
	UFUNCTION(BlueprintCallable)
	void SpawnHammerDownParticles();
};