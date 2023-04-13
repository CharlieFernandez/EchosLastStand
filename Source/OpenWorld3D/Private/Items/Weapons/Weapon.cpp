// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Weapons/Weapon.h"

#include "Characters/OpenWorldCharacter/OpenWorldCharacter.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NiagaraComponent.h"

AWeapon::AWeapon()
{
	DamageDealerComponent = CreateDefaultSubobject<UDamageDealer>(TEXT("Damage Dealer Cmpt"));
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AWeapon::ToggleWeaponState()
{
	ItemState = (ItemState == EItemState::EIS_Hovering) ? EItemState::EIS_Held : EItemState::EIS_Hovering;

	if(ItemState == EItemState::EIS_Held)
	{		
		SetToHeldItem();
	}
	else
	{
		SphereComponent->SetGenerateOverlapEvents(true);
	}
}

void AWeapon::SetToHeldItem()
{
	ItemState = EItemState::EIS_Held;
	SphereComponent->SetGenerateOverlapEvents(false);
	PickUpParticles->Deactivate();
	ItemMesh->EmptyOverrideMaterials();
}