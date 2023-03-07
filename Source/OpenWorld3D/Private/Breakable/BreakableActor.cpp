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

void ABreakableActor::GetHit_Implementation(const FVector ImpactPoint)
{
	if(UWorld* World = GetWorld())
	{
		FVector SpawnLocation = GetActorLocation();
		SpawnLocation.Z = 32.f;
		World->SpawnActor<ATreasure>(Treasure, SpawnLocation, GetActorRotation());		
	}

	GeometryCollectionComponent -> SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}

