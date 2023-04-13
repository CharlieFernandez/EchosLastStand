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
	void ObtainOrEquip();
	virtual void Jump() override;
	bool HasMovementInput() const;
	virtual void BeginPlay() override;
	void Move(const FInputActionValue& Value);
	void Roll(const FInputActionValue& Value);
	void LookAround(const FInputActionValue& Value);
	virtual void GetHit_Implementation(const FVector ImpactPoint, const FVector InstigatorPosition) override;

	
	/* Properties */	
	UPROPERTY(EditDefaultsOnly, Category = Input) TObjectPtr<UInputMappingContext> OpenWorldCharacterContext;	
	UPROPERTY(EditDefaultsOnly, Category = Input) TObjectPtr<UInputAction> MovementAction;
	UPROPERTY(EditDefaultsOnly, Category = Input) TObjectPtr<UInputAction> LookAroundAction;
	UPROPERTY(EditDefaultsOnly, Category = Input) TObjectPtr<UInputAction> JumpAction;
	UPROPERTY(EditDefaultsOnly, Category = Input) TObjectPtr<UInputAction> EKeyPressedAction;
	UPROPERTY(EditDefaultsOnly, Category = Input) TObjectPtr<UInputAction> AttackPressedAction;
	UPROPERTY(EditDefaultsOnly, Category = Input) TObjectPtr<UInputAction> RollAction;
	UPROPERTY(EditDefaultsOnly, Category = Input) TObjectPtr<UInputAction> LockOnAction;

	/* Functions */
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


private:
	/* Fields */
	FVector DirectionToRoll;
	inline static const FName PlayerTag = FName("Player");
	TObjectPtr<UOpenWorldCharacterHUD> OpenWorldCharacterHUD;

	/* Methods */
	void BindHealthRadiusSphereComponents();
	void InitializeHUD(const APlayerController* PlayerController);
	void EquipOrUnequipWeapon();
	void PlayRollMontage(const FInputActionValue& Value);
	void PlayEquipMontage(EEquipActionState EquipType) const;
	void PickUpWeapon(AWeapon* Weapon, UMeshComponent* WeaponMesh, FName SN);
	void ToggleLockOn();

	
	/* Properties */
	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<ULockOnComponent> LockOnComponent;
	
	UPROPERTY(VisibleAnywhere)
	USphereComponent* HealthRadiusSphereComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> CameraBoom;
	
	UPROPERTY(EditDefaultsOnly)
	float RollSpeed;
	
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<AItem> OverlappedItem;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> EquipMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> RollMontage;

	UPROPERTY(EditDefaultsOnly, Category = Camera)
	float CameraRotationSpeed;

	UPROPERTY(EditDefaultsOnly, Category = Stamina)
	float DodgeRollStaminaCost;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Particles, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UNiagaraSystem> HammerDownParticles;

	/* Functions */
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void SpeedUpRoll();

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void SlowDownRoll();

	UFUNCTION(BlueprintCallable)
	void SpawnHammerDownParticles();
};