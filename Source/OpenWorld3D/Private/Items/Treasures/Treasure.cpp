// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Treasures/Treasure.h"
#include "Interfaces/PickUpInterface.h"

void ATreasure::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(IPickUpInterface* PickUpInterface = Cast<IPickUpInterface>(OtherActor))
	{
		PickUpInterface->AddGold(this);
		PickUpItem();
	}
}
