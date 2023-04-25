// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/LockableComponent.h"
#include "NiagaraComponent.h"

ULockableComponent::ULockableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void ULockableComponent::BeginPlay()
{
	Super::BeginPlay();
	LockOnNiagaraComponent = NewObject<UNiagaraComponent>(this, LockOnNiagaraComponentClass);
	LockOnNiagaraComponent->RegisterComponent();
	LockOnNiagaraComponent->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
}

UNiagaraComponent* ULockableComponent::ActivateLockOnNS() const
{
	LockOnNiagaraComponent->Activate();
	return LockOnNiagaraComponent;
}