// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OPENWORLD3D_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void UpdateHealth(float HealthToAdd);
	void UpdateStamina(float StaminaToAdd);
	FORCEINLINE float GetCurrentHealthPercent() const { return CurrentHealth / MaxHealth; };
	FORCEINLINE float GetCurrentStaminaPercent() const { return CurrentStamina / MaxStamina; };
	FORCEINLINE float GetCurrentStamina() const { return CurrentStamina; };
	void AddGold(int32 GoldValue);
	void AddSouls(int32 SoulsValue);
	FORCEINLINE int32 GetTotalGold() const { return Gold; }
	FORCEINLINE int32 GetTotalSouls() const { return Souls; }
	void RegenerateStamina(float DeltaTime);

	UFUNCTION(BlueprintPure)
	bool IsAlive() const { return CurrentHealth > 0; }

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly)
	float MaxHealth;
	UPROPERTY(EditDefaultsOnly)
	float MaxStamina;
	UPROPERTY(VisibleInstanceOnly)
	float CurrentHealth;
	UPROPERTY(VisibleInstanceOnly)
	float CurrentStamina;
	UPROPERTY(EditDefaultsOnly)
	float StaminaRegenPerSecond;
	UPROPERTY(EditDefaultsOnly)
	int32 Gold;
	UPROPERTY(EditDefaultsOnly)
	int32 Souls;
};
