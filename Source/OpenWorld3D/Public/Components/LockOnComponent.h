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
	void InitializeVariables();
	void InitializeVariablesOnBeginPlay(float DefaultCameraDistance);
	TObjectPtr<AActor> LockedOnTarget;	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void Unlock();
	void Lock();

protected:
	virtual void BeginPlay() override;

private:
	float DefaultCameraDistance;	
	TObjectPtr<APlayerController> PlayerController;
	TObjectPtr<UNiagaraComponent> CurrentlyUsedLockOnNC;
	TObjectPtr<USpringArmComponent> SpringArmComponent;	
	TObjectPtr<UCameraComponent> CameraComponent;
	
	TArray<AEnemy*> FindAndFilterEnemies() const;
	void SetLockOnTarget(const TArray<AEnemy*>Enemies);

	UPROPERTY(EditDefaultsOnly)
	float SpringArmExtraSlack;

	UPROPERTY(VisibleInstanceOnly, Category = Targeting)
	TObjectPtr<USceneComponent> MidPointLockOn;

	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<USphereComponent> MidPointSphereCollider;

	UPROPERTY(EditDefaultsOnly, Category = Targeting)
	float LockOnRadius;

	UPROPERTY(EditDefaultsOnly, Category = Targeting)
	float MinCameraDistance;

	UPROPERTY(EditDefaultsOnly, Category = Targeting)
	float MaxDistanceFromLockedTarget;
};
