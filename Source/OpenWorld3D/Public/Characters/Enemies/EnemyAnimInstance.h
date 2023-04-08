// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Characters/GameCharacterAnimInstance.h"
#include "EnemyAnimInstance.generated.h"

class UCharacterMovementComponent;

class AEnemy;
class UCharacterMovementComponent;

UCLASS()
class OPENWORLD3D_API UEnemyAnimInstance : public UGameCharacterAnimInstance
{
	GENERATED_BODY()

private:	
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AEnemy> Enemy;
};
