// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LockOnComponent.generated.h"


class UNiagaraComponent;
class USpringArmComponent;
class UCameraComponent;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OPENWORLD3D_API ULockOnComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULockOnComponent();
	void InitializeVariables() const;
	void InitializeVariablesOnBeginPlay(float DefaultCameraDistance);
	TObjectPtr<AActor> LockedOnTarget;	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ToggleLockOntoTarget();
	void Unlock();
	void Lock();

protected:
	virtual void BeginPlay() override;

private:
	float DefaultCameraDistance;
	float LockOnRadius;	
	TObjectPtr<APlayerController> PlayerController;
	TObjectPtr<UNiagaraComponent> CurrentlyUsedLockOnNC;
	TObjectPtr<USpringArmComponent> SpringArmComponent;	
	TObjectPtr<UCameraComponent> CameraComponent;
	
	void FindAndFilterEnemies(TArray<AActor*> ActorsToIgnore, TArray<FHitResult>& HitResults) const;
	void SetLockOnTarget(const TArray<FHitResult>& HitResults);

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> MidPointLockOn;

	UPROPERTY(EditDefaultsOnly)
	float MinCameraDistance;

	UPROPERTY(EditDefaultsOnly)
	float MaxCameraDistance;
};
