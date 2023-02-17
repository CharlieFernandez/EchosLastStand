// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "OpenWorld3D/DebugMacros.h"

// Sets default values
AItem::AItem(): Amplitude(0.25f), TimeConstant(5.f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	RootComponent = ItemMesh;
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
}

float AItem::TransformSine()
{
	return Amplitude * FMath::Sin(RunningTime * TimeConstant);
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;

	AddActorWorldRotation(FRotator(0, 50, 0) * DeltaTime);
}

