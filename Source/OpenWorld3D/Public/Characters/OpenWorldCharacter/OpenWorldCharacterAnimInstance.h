// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Characters/CharacterTypes.h"
#include "Characters/GameCharacterAnimInstance.h"
#include "OpenWorldCharacterAnimInstance.generated.h"


class AOpenWorldCharacter;
class UCharacterMovementComponent;

UCLASS()
class OPENWORLD3D_API UOpenWorldCharacterAnimInstance : public UGameCharacterAnimInstance
{
	GENERATED_BODY()

private:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AOpenWorldCharacter> OpenWorldCharacter;
};
