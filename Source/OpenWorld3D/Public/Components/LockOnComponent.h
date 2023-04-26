// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LockOnComponent.generated.h"


class AMidPointLockOn;
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
	TObjectPtr<AActor> LockedOnTarget;	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ToggleLockOntoTarget();
	void Unlock();


protected:
	virtual void BeginPlay() override;

private:
	TObjectPtr<APlayerController> PlayerController;
	TObjectPtr<UNiagaraComponent> CurrentlyUsedLockOnNC;
	
	void Lock();
	void FindAndFilterEnemies(TArray<AActor*> ActorsToIgnore, TArray<FHitResult>& HitResults) const;
	void SetLockOnTarget(const TArray<FHitResult>& HitResults);

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> MidPointLockOn;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(EditDefaultsOnly)
	FVector LockOnOffset;

	UPROPERTY(EditDefaultsOnly)
	float LockOnRadius = 1000.f;
};
