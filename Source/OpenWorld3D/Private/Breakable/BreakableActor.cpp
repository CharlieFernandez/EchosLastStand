// Fill out your copyright notice in the Description page of Project Settings.


#include "Breakable/BreakableActor.h"

#include "Components/CapsuleComponent.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Items/Treasures/Treasure.h"

ABreakableActor::ABreakableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	GeometryCollectionComponent = CreateDefaultSubobject<UGeometryCollectionComponent>("Geometry Collection");
	GeometryCollectionComponent -> SetGenerateOverlapEvents(true);
	GeometryCollectionComponent -> SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	SetRootComponent(GeometryCollectionComponent);

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	CapsuleComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	CapsuleComponent->SetupAttachment(GeometryCollectionComponent);
}

void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABreakableActor::GetHit_Implementation(const FVector ImpactPoint, const FVector InstigatorPosition)
{
	if(UWorld* World = GetWorld())
	{
		if(const uint32 NumOfTreasures = Treasures.Num())
		{
			if(NumOfTreasures > 0)
			{
				const int32 IndexSelection = FMath::RandRange(0, NumOfTreasures - 1);
				const TSubclassOf<ATreasure> TreasureToSpawn = Treasures[IndexSelection];
				FVector SpawnLocation = GetActorLocation();

				const EItemState ItemState = TreasureToSpawn.GetDefaultObject()->GetItemState();
				if(ItemState == EItemState::EIS_Hovering)
				{
					SpawnLocation.Z += 90.f;
				}
				else if(ItemState == EItemState::EIS_Grounded)
				{
					SpawnLocation.Z = 32.f;
				}			
			
				World->SpawnActor<ATreasure>(TreasureToSpawn, SpawnLocation, GetActorRotation());
			}
		}		
	}

	GeometryCollectionComponent -> SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}