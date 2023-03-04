// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Items/Item.h"
#include "Characters/CharacterTypes.h"
#include "Items/Weapons/EquipActionState.h"
#include "Items/Weapons/Weapon.h"

#include "OpenWorldCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class AItem;
class UAnimMontage;
class UCharacterMovementComponent;
class AWeapon;
class UAnimInstance;

UCLASS()
class OPENWORLD3D_API AOpenWorldCharacter : public ACharacter
{
	GENERATED_BODY()

public:	
	// Default Methods	
	AOpenWorldCharacter();	
	virtual void Tick(float DeltaTime) override;	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// Getter/Setter Methods
	FORCEINLINE EEquipState GetCharacterState() const { return EquipState; }
	FORCEINLINE void SetOverlappingItem(AItem* Item){ OverlappedItem = Item; }
	
protected:
	// Input Mapping Contexts	
	UPROPERTY(EditDefaultsOnly, Category = Input)
	UInputMappingContext* OpenWorldCharacterContext;

	// Input Actions	
	UPROPERTY(EditDefaultsOnly, Category = Input) UInputAction* MovementAction;
	UPROPERTY(EditDefaultsOnly, Category = Input) UInputAction* LookAroundAction;
	UPROPERTY(EditDefaultsOnly, Category = Input) UInputAction* JumpAction;
	UPROPERTY(EditDefaultsOnly, Category = Input) UInputAction* EKeyPressedAction;
	UPROPERTY(EditDefaultsOnly, Category = Input) UInputAction* AttackPressedAction;
	UPROPERTY(EditDefaultsOnly, Category = Input) UInputAction* SprintAction;
	
	// Default Methods
	virtual void BeginPlay() override;
	
	// Input Methods
	void Move(const FInputActionValue& Value);
	void LookAround(const FInputActionValue& Value);
	virtual void Jump() override;
	void EKeyPressed();
	void SprintPressed();
	void Attack();

	

private:
	// Pointers
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<AItem> OverlappedItem;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AWeapon> WeaponHeld;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> CameraBoom;	
	
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> EquipMontage;

	TObjectPtr<UAnimInstance> AnimInstance;

	TObjectPtr<UCharacterMovementComponent> CharacterMovementComponent;
	
	// States
	UPROPERTY(VisibleInstanceOnly)
	EEquipState EquipState = EEquipState::ECS_Unequipped;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	// Character Movement Speeds
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MaxWalkSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MaxRunSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MaxSprintSpeed;

	// Notify Methods
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void AttackEnd();

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void ResetActionState();

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void Equip();

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void Unequip();

	// Helper Methods
	bool CanAttack();
	
	// Montage Methods
	void PlayActionMontage() const;
	void PlayEquipMontage(EEquipActionState EquipType);

	// Helper Methods
	void PickUpWeapon(AWeapon* Weapon, UMeshComponent* WeaponMesh, FName SN);
	bool CanEquip() const;
	bool CanUnequip() const;
	void AttachMeshToSocket(UMeshComponent* WeaponMesh, FName SN);
};