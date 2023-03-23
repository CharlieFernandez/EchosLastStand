// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponTypes.h"
#include "Damage Dealer/DamageDealer.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

class UBoxComponent;

UCLASS()
class OPENWORLD3D_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();
	void ToggleWeaponState();
	
	void SetToHeldItem();
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDamageDealer> DamageDealerComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponType;
};