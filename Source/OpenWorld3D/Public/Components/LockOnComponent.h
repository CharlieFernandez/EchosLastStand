// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LockOnComponent.generated.h"


class USpringArmComponent;
class UCameraComponent;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OPENWORLD3D_API ULockOnComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULockOnComponent();
	TObjectPtr<AActor> LockedOnTarget;
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ToggleLockOntoTarget();


protected:
	virtual void BeginPlay() override;

private:
	void Lock();
	void Unlock();
	void FindAndFilterEnemies(TArray<AActor*> ActorsToIgnore, TArray<FHitResult>& HitResults) const;
	void SetLockOnTarget(const TArray<FHitResult>& HitResults);



	UPROPERTY(EditDefaultsOnly)
	FVector LockOnOffset;

	UPROPERTY(EditDefaultsOnly)
	float LockOnRadius = 1000.f;
};
