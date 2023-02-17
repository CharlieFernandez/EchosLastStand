// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "OpenWorld3D/DebugMacros.h"

// Sets default values
AItem::AItem(): Amplitude(0.25f), TimeConstant(5.f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	//DRAW_SPHERE (ActorLocation);
	//DRAW_VECTOR(ActorLocation, ActorLocation + ActorForward * 100);

	int32 AverageInt = Avg<int32>(5, 3);
	UE_LOG(LogTemp, Warning, TEXT("Average: %d"), AverageInt);
	FVector AverageVector = Avg<FVector>(GetActorLocation(), FVector::Zero());
	UE_LOG(LogTemp, Warning, TEXT("Average Vector: %s"), *AverageVector.ToString());
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

	FVector ActorLocation = GetActorLocation();
	FVector ActorForward = GetActorForwardVector();
	DRAW_SPHERE_SINGLE_FRAME(ActorLocation);
	DRAW_VECTOR_SINGE_FRAME(ActorLocation, ActorLocation + ActorForward * 100);
}

