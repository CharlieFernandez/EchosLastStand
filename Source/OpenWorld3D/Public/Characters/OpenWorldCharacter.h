// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameCharacter.h"
#include "InputActionValue.h"
#include "Items/Item.h"
#include "Characters/CharacterTypes.h"
#include "Items/Weapons/EquipActionState.h"

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

UCLASS(BlueprintType)
class OPENWORLD3D_API AOpenWorldCharacter : public AGameCharacter
{
	GENERATED_BODY()

public:	
	/* Methods */
	AOpenWorldCharacter();	
	virtual void Tick(float DeltaTime) override;	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	FORCEINLINE static FName GetPlayerTag() { return PlayerTag; }
	FORCEINLINE EEquipState GetCharacterState() const { return EquipState; }
	FORCEINLINE void SetOverlappingItem(AItem* Item){ OverlappedItem = Item; }
	
protected:
	/* Methods */
	void ObtainOrEquip();
	virtual void Jump() override;
	virtual void BeginPlay() override;
	void Move(const FInputActionValue& Value);
	void Roll(const FInputActionValue& Value);
	void LookAround(const FInputActionValue& Value);
	
	/* Properties */	
	UPROPERTY(EditDefaultsOnly, Category = Input) TObjectPtr<UInputMappingContext> OpenWorldCharacterContext;	
	UPROPERTY(EditDefaultsOnly, Category = Input) TObjectPtr<UInputAction> MovementAction;
	UPROPERTY(EditDefaultsOnly, Category = Input) TObjectPtr<UInputAction> LookAroundAction;
	UPROPERTY(EditDefaultsOnly, Category = Input) TObjectPtr<UInputAction> JumpAction;
	UPROPERTY(EditDefaultsOnly, Category = Input) TObjectPtr<UInputAction> EKeyPressedAction;
	UPROPERTY(EditDefaultsOnly, Category = Input) TObjectPtr<UInputAction> AttackPressedAction;
	UPROPERTY(EditDefaultsOnly, Category = Input) TObjectPtr<UInputAction> SprintAction;
	UPROPERTY(EditDefaultsOnly, Category = Input) TObjectPtr<UInputAction> RollAction;

	/* Functions */
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


private:
	/* Fields */
	FVector DirectionToRoll;
	inline static const FName PlayerTag = FName("Player");

	/* Methods */
	void EquipOrUnequipWeapon();
	virtual bool CanAttack() override;
	void PlayRollMontage(const FInputActionValue& Value);	
	void PlayEquipMontage(EEquipActionState EquipType) const;
	void PickUpWeapon(AWeapon* Weapon, UMeshComponent* WeaponMesh, FName SN);

	
	/* Properties */
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

	/* Functions */
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void SpeedUpRoll();

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void SlowDownRoll();
	
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void ResetActionState();
};