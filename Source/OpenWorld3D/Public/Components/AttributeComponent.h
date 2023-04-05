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
	float GetCurrentHealthPercent() const;
	void AddGold(int32 GoldValue);
	void AddSouls(int32 SoulsValue);
	FORCEINLINE int32 GetTotalGold() const { return TotalGold; }
	FORCEINLINE int32 GetTotalSouls() const { return TotalSouls; }

	UFUNCTION(BlueprintPure)
	bool IsAlive() const { return CurrentHealth > 0; }

protected:
	virtual void BeginPlay() override;

private:
	int32 TotalGold;
	int32 TotalSouls;
	
	UPROPERTY(VisibleInstanceOnly)
	float CurrentHealth;
	UPROPERTY(EditDefaultsOnly)
	float MaxHealth;
};
