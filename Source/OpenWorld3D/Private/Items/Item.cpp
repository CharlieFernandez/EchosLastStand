// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "OpenWorld3D/DebugMacros.h"

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

	FVector NewActorLocation = FVector(0.f, 0.f, 100.f);
	SetActorLocation(NewActorLocation);
	SetActorRotation(FRotator(0.f, 90.f, 0.f));
	FVector ActorForward = GetActorForwardVector();

	DRAW_SPHERE (NewActorLocation);
	DRAW_VECTOR(NewActorLocation, NewActorLocation + ActorForward * 100);
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

