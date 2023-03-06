// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Weapons/Weapon.h"
#include "Characters/OpenWorldCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Interfaces/HitInterface.h"
#include "Kismet/KismetSystemLibrary.h"

AWeapon::AWeapon()
{
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Hit Box"));
	BoxComponent->SetupAttachment(GetRootComponent());

	BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	BoxComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Start Component"));
	BoxTraceStart->SetupAttachment(GetRootComponent());
	
	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("End Component"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxBeginOverlap);
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
	FVector Start = BoxTraceStart->GetComponentLocation();
	FVector End = BoxTraceEnd->GetComponentLocation();
	
	ActorsToIgnore.AddUnique(this);

	FHitResult HitResult;
	
	UKismetSystemLibrary::BoxTraceSingle(
		this,
		Start,
		End,
		FVector(5.f, 5.f, 5.f),
		BoxTraceStart->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		HitResult,
		true
	);	

	if(AActor* ActorHit = HitResult.GetActor())
	{
		ActorsToIgnore.AddUnique(ActorHit);
		
		if(IHitInterface* HitInterface = Cast<IHitInterface>(ActorHit))
		{
			for(AActor* Actor: ActorsToIgnore)
			{
				GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2, FColor::Emerald, Actor->GetName());				
			}
			
			HitInterface->GetHit(HitResult.ImpactPoint);
		}

		CreateFields(HitResult.ImpactPoint);
	}
}

void AWeapon::ToggleWeaponState()
{
	ItemState = (ItemState == EItemState::EIS_Hovering) ? EItemState::EIS_Held : EItemState::EIS_Hovering;

	if(ItemState == EItemState::EIS_Held)
	{		
		SphereComponent->SetGenerateOverlapEvents(false);
	}
	else
	{
		SphereComponent->SetGenerateOverlapEvents(true);
	}
}

void AWeapon::SetHitBoxCollisionType(ECollisionEnabled::Type CollisionType)
{
	if(BoxComponent)
	{
		BoxComponent->SetCollisionEnabled(CollisionType);

		if(CollisionType == ECollisionEnabled::NoCollision)
		{
			ActorsToIgnore.Empty();
		}		
	}
}