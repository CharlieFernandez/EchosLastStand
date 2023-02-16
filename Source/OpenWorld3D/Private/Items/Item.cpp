// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "DrawDebugHelpers.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	
	UWorld* World = GetWorld();
	FVector ActorLocation = GetActorLocation();
	if (World)
	{
		FVector ActorForward = GetActorForwardVector();
		DrawDebugSphere(World, ActorLocation, 25.f, 36, FColor::Blue, false, 30.f);
		DrawDebugLine(World, ActorLocation, ActorLocation + (ActorForward * 100), FColor::Red, true, -1.f, 0u, 1.f);
	}

}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

