// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AttributeComponent.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	CurrentHealth = MaxHealth;
	CurrentStamina = MaxStamina;
}

void UAttributeComponent::BeginPlay()
{
	CurrentHealth = MaxHealth;
	CurrentStamina = MaxStamina;
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

void UAttributeComponent::UpdateStamina(float StaminaToAdd)
{
	CurrentStamina = FMath::Clamp(CurrentStamina + StaminaToAdd, 0, MaxStamina);
}

void UAttributeComponent::AddGold(int32 GoldValue)
{
	Gold += GoldValue;
}

void UAttributeComponent::AddSouls(int32 SoulsValue)
{
	Souls += SoulsValue;
}

void UAttributeComponent::RegenerateStamina(float DeltaTime)
{
	CurrentStamina = FMath::Clamp(CurrentStamina + StaminaRegenPerSecond * DeltaTime, 0, MaxStamina);
}
