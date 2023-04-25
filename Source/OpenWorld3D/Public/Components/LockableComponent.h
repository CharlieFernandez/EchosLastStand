// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LockableComponent.generated.h"

class UNiagaraComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OPENWORLD3D_API ULockableComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULockableComponent();
	UNiagaraComponent* ActivateLockOnNS() const;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UNiagaraComponent> LockOnNiagaraComponentClass;
	TObjectPtr<UNiagaraComponent> LockOnNiagaraComponent;
};
