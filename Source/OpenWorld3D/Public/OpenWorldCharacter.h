// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"

#include "OpenWorldCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class OPENWORLD3D_API AOpenWorldCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AOpenWorldCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Jump() override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	UInputMappingContext* OpenWorldCharacterContext;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	UInputAction* MovementAction;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	UInputAction* LookAroundAction;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	UInputAction* JumpAction;;

	void Move(const FInputActionValue& Value);
	void LookAround(const FInputActionValue& Value);

private:
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* Ptr_SpringArm;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Ptr_Camera;
};
