// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AttributeComponent.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAttributeComponent::BeginPlay()
{
	CurrentHealth = MaxHealth;
	Super::BeginPlay();
}

void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAttributeComponent::UpdateHealth(float HealthToAdd)
{
	CurrentHealth = FMath::Clamp(CurrentHealth + HealthToAdd, 0, MaxHealth);
}

float UAttributeComponent::GetCurrentHealthPercent() const
{
	return CurrentHealth / MaxHealth;
}