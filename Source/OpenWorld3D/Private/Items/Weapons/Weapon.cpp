// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Weapons/Weapon.h"

#include "Enemy.h"
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
		SetToHeldItem<AOpenWorldCharacter>();
	}
	else
	{
		SphereComponent->SetGenerateOverlapEvents(true);
	}
}

template <class T>
void AWeapon::SetToHeldItem()
{
	ItemState = EItemState::EIS_Held;
	SphereComponent->SetGenerateOverlapEvents(false);
	PickUpParticles->Deactivate();
	ItemMesh->EmptyOverrideMaterials();

	if(typeid(T) == typeid(AEnemy))
	{
		TArray<FDamageHitBox> AllHitBoxes = DamageDealerComponent->GetAllDamageHitBoxes();

		for(FDamageHitBox DamageHitBox: AllHitBoxes)
		{
			UPrimitiveComponent* HitBox = DamageHitBox.GetHitBox();
			HitBox->SetGenerateOverlapEvents(true);
			HitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			HitBox->SetCollisionResponseToAllChannels(ECR_Ignore);
			HitBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		}
	}
}

template void AWeapon::SetToHeldItem<AEnemy>();
template void AWeapon::SetToHeldItem<AOpenWorldCharacter>();