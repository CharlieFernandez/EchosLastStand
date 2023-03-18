// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Weapons/Weapon.h"
#include "Characters/OpenWorldCharacter/OpenWorldCharacter.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"

AWeapon::AWeapon()
{
	DamageDealerComponent = CreateDefaultSubobject<UDamageDealer>(TEXT("Damage Dealer Cmpt"));
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	for(UPrimitiveComponent* HitBox: AllHitBoxes)
	{
		HitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		HitBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		HitBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);	
	}

	Damage = CurrentlyAllDamage;
	for(UPrimitiveComponent* PrimitiveComponent: AllHitBoxes)
	{
		PrimitiveComponent->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxBeginOverlap);
	}
}

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

void AWeapon::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{	
	DamageDealerComponent -> DealDamage(
		OverlappedComponent,
		OverlappedComponent->GetChildComponent(0)->GetComponentLocation(),
		OverlappedComponent->GetChildComponent(1)->GetComponentLocation(),
		Damage,
		HitSFX
	);
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