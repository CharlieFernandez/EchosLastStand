// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"
#include "Characters/OpenWorldCharacter.h"
#include "Components/SphereComponent.h"

void AWeapon::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void AWeapon::ToggleWeaponState()
{
	WeaponState = (WeaponState == EWeaponState::EWS_Hovering) ? EWeaponState::EWS_Held : EWeaponState::EWS_Hovering;

	if(WeaponState == EWeaponState::EWS_Held)
	{		
		SphereComponent->SetGenerateOverlapEvents(false);
	}
	else
	{
		SphereComponent->SetGenerateOverlapEvents(true);
	}
}
