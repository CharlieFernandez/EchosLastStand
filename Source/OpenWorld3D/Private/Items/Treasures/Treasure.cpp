// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Treasures/Treasure.h"
#include "Characters/OpenWorldCharacter.h"
#include "Kismet/GameplayStatics.h"

void ATreasure::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(AOpenWorldCharacter* OpenWorldCharacter = Cast<AOpenWorldCharacter>(OtherActor))
	{
		if(PickUpSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, PickUpSound, GetActorLocation());
			Destroy();
		}
	}
}

void ATreasure::Tick(float DeltaTime)
{
	
}
