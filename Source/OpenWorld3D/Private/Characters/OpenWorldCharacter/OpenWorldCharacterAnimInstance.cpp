// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/OpenWorldCharacter/OpenWorldCharacterAnimInstance.h"

#include "Characters/OpenWorldCharacter/OpenWorldCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UOpenWorldCharacterAnimInstance::NativeInitializeAnimation()
{
	OpenWorldCharacter = Cast<AOpenWorldCharacter>(TryGetPawnOwner());
}

void UOpenWorldCharacterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
}