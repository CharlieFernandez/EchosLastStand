// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/GameCharacterAnimInstance.h"

#include "Characters/GameCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Kismet/KismetMathLibrary.h"

void UGameCharacterAnimInstance::NativeInitializeAnimation()
{
	GameCharacter = Cast<AGameCharacter>(TryGetPawnOwner());
	
	if (GameCharacter)
	{
		CharacterMovementComponent = GameCharacter->GetCharacterMovement();
		Attributes = GameCharacter->GetAttributes();
	}
}

void UGameCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if(CharacterMovementComponent)
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(CharacterMovementComponent -> Velocity);
		IsFalling = CharacterMovementComponent->IsFalling();

		if(GameCharacter)
		{
			EquipState = GameCharacter->GetCharacterState();
		}
	}
}

EWeaponType UGameCharacterAnimInstance::GetWeaponHeldType() const
{
	if(GameCharacter)
	{
		if(const AWeapon* WeaponHeld = GameCharacter->GetWeaponHeld())
		{
			return WeaponHeld->GetWeaponType();
		}
	}

	return EWeaponType::EWT_OneHanded; //TODO: Check on this later
}

UBlendSpace1D* UGameCharacterAnimInstance::EvaluateGroundLocomotionBlendspace() const
{
	UBlendSpace1D* ChosenBlendspace = UnequippedBlendspace;
	
	switch (EquipState)
	{
		case EEquipState::ECS_Unequipped:
			ChosenBlendspace = UnequippedBlendspace;
			break;
		case EEquipState::ECS_Equipped:
			const EWeaponType WeaponType = GetWeaponHeldType();
			ChosenBlendspace = WeaponType == EWeaponType::EWT_OneHanded ?  OneHandedBlendspace : TwoHandedBlendspace;
			break;
	}

	return ChosenBlendspace;
}

UAnimSequenceBase* UGameCharacterAnimInstance::SelectRandomDeathSequence()
{
	if(AllDeathSequences.Num() > 0)
	{
		const uint32 SelectedIndex = FMath::RandHelper(AllDeathSequences.Num());
		return AllDeathSequences[SelectedIndex];
	}

	return TObjectPtr<UAnimSequenceBase>();
}