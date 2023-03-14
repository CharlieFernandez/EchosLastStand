// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameCharacter.generated.h"

UCLASS(Abstract)
class OPENWORLD3D_API AGameCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AGameCharacter();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Visual Effects")
	TObjectPtr<UParticleSystem> HitParticles;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MaxWalkSpeed;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UAnimInstance> AnimInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MaxRunSpeed;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
