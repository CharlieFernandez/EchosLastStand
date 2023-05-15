// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LockOnComponent.generated.h"


class USphereComponent;
class UNiagaraComponent;
class USpringArmComponent;
class UCameraComponent;
class AEnemy;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OPENWORLD3D_API ULockOnComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULockOnComponent();
	void InitializeVariablesOnBeginPlay(float DefaultCameraDistance);
	TObjectPtr<AActor> LockedOnTarget;	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ActivateTargetingVFX();
	void Unlock();
	void Lock();
	void SwitchTargetRight();
	void SwitchTargetLeft();

protected:
	virtual void BeginPlay() override;
	void EvaluateVectorsForDotProduct();
	void EvaluateSpringArmLengthMultiplier();
	void SetSpringArmLength();
	void SetSpringArmTargetOffset();
	void AdjustCameraDuringLockOn();
	void AdjustCameraWhenUnlocking() const;

private:
	FVector PlayerLocation;
	FVector EnemyLocation;
	FVector CameraToPlayerNormalized;
	FVector PlayerToEnemyNormalized;
	const float Alpha = 0.05f;
	const float HalfDistanceMultiplierForCameraOffset = 0.5f;
	float DefaultCameraDistance;
	float Dot;
	float InvertedDot;
	float SpringArmLengthMultiplier;
	float DistanceBetweenPlayerAndEnemy;		
	float DesiredSpringArmLength;
	float CameraBackModifier;
	TObjectPtr<UNiagaraComponent> CurrentlyUsedLockOnNC;
	TObjectPtr<USpringArmComponent> SpringArmComponent;	
	TObjectPtr<UCameraComponent> CameraComponent;
	void EvaluateSpringArmAndCameraProperties();

	
	TArray<AEnemy*> FindAndFilterEnemies() const;
	void SetLockOnTarget(const TArray<AEnemy*>Enemies);

	UPROPERTY(EditDefaultsOnly)
	float SpringArmExtraSlack;

	UPROPERTY(EditDefaultsOnly, Category = Targeting)
	float LockOnRadius;

	UPROPERTY(EditDefaultsOnly, Category = Targeting)
	float MinCameraDistance;

	UPROPERTY(EditDefaultsOnly, Category = Targeting)
	float MaxDistanceFromLockedTarget;

	UPROPERTY(EditDefaultsOnly, Category = Targeting)
	TObjectPtr<USphereComponent> LockOnSensor;
};
