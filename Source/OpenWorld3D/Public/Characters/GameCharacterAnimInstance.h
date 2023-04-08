// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterTypes.h"
#include "Animation/AnimInstance.h"
#include "Items/Weapons/WeaponTypes.h"
#include "GameCharacterAnimInstance.generated.h"

class UBlendSpace1D;
class UAttributeComponent;
class AGameCharacter;
class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class OPENWORLD3D_API UGameCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UCharacterMovementComponent> CharacterMovementComponent;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float GroundSpeed;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool IsFalling;

	UPROPERTY(BlueprintReadOnly, Category = Weapon)
	EEquipState EquipState;

	UPROPERTY(BlueprintReadOnly)
	UAttributeComponent* Attributes;

	UFUNCTION(BlueprintPure)
	UBlendSpace1D* EvaluateGroundLocomotionBlendspace() const;

	UFUNCTION(BlueprintCallable)
	UAnimSequenceBase* SelectRandomDeathSequence();

private:
	TObjectPtr<AGameCharacter> GameCharacter;
	
	EWeaponType GetWeaponHeldType() const;

	UPROPERTY(BlueprintReadWrite, Category = Blendspaces, meta = (AllowPrivateAccess = "true"))
	UBlendSpace1D* UnequippedBlendspace;

	UPROPERTY(BlueprintReadWrite, Category = Blendspaces, meta = (AllowPrivateAccess = "true"))
	UBlendSpace1D* OneHandedBlendspace;

	UPROPERTY(BlueprintReadWrite, Category = Blendspaces, meta = (AllowPrivateAccess = "true"))
	UBlendSpace1D* TwoHandedBlendspace;

	UPROPERTY(BlueprintReadWrite, Category = Death, meta = (AllowPrivateAccess = "true"))
	TArray<UAnimSequenceBase*> AllDeathSequences;
};
