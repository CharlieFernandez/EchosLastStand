// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Item.h"

#include "MyUtilities.h"
#include "NiagaraComponent.h"
#include "Interfaces/PickUpInterface.h"
#include "Components/SphereComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Items/Weapons/Weapon.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AItem::AItem(): Amplitude(0.25f), TimeConstant(5.f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	RootComponent = ItemMesh;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Comp"));
	SphereComponent -> SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereBeginOverlap);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);

	if(GetOwner() == nullptr)
	{
		PlayUncollectedSound();

		if(UncollectedParticles)
		{
			const FVector StartLocation = GetActorLocation();
			FVector EndLocation = StartLocation;
			EndLocation.Z -= 200.f;

			const EDrawDebugTrace::Type DrawDebugTrace = DrawItemRingLineTrace ? EDrawDebugTrace::Persistent : EDrawDebugTrace::None;			
			const FHitResult FloorHit = MyUtilities::GetLineTraceGroundImpactPoint(this, StartLocation, EndLocation, DrawDebugTrace);

			if(FloorHit.GetActor() != nullptr)
			{
				UncollectedParticlesInGame = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, UncollectedParticles, FloorHit.ImpactPoint);
			}
		}
	}

	
}

void AItem::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(IPickUpInterface* PickUpInterface = Cast<IPickUpInterface>(OtherActor))
	{
		PickUpInterface->SetOverlappingItem(this);
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(IPickUpInterface* PickUpInterface = Cast<IPickUpInterface>(OtherActor))
	{
		PickUpInterface -> SetOverlappingItem(nullptr);
	}
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;

	if(ItemState == EItemState::EIS_Hovering)
	{
		AddActorWorldOffset(FVector(0, 0, TransformSine()));
		AddActorLocalRotation(FRotator(0, SpinSpeed * DeltaTime, 0));
	}
}

void AItem::SpawnPickUpEffect() const
{
	if(CollectedParticles)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			CollectedParticles,
			GetActorLocation()
		);
	}
}

void AItem::PickUpItem()
{
	SpawnPickUpEffect();
	StopUncollectedSound();
	PlayPickUpSound();

	if(!Cast<AWeapon>(this))
	{
		Destroy();
	}
}

void AItem::PlayUncollectedSound()
{
	if(UncollectedSound && ItemState != EItemState::EIS_Held)
	{
		UncollectedSoundPlayed = UGameplayStatics::SpawnSoundAtLocation(this, UncollectedSound, GetActorLocation());
	}
}

void AItem::StopUncollectedSound() const
{
	if(UncollectedSoundPlayed)
	{
		UncollectedSoundPlayed->Stop();
	}
}

void AItem::PlayPickUpSound() const
{
	if(PickUpSound == nullptr) return;

	if(UncollectedSoundPlayed)
	{
		StopUncollectedSound();
	}
	
	UGameplayStatics::PlaySoundAtLocation(this, PickUpSound, GetActorLocation());
}
