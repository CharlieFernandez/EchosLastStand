// Fill out your copyright notice in the Description page of Project Settings.


#include "Damage Dealer/DamageDealer.h"

#include "Components/ShapeComponent.h"
#include "Field/FieldSystemComponent.h"
#include "Interfaces/HitInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

UDamageDealer::UDamageDealer()
{
	PrimaryComponentTick.bCanEverTick = true;

	FieldSystemComponent = CreateDefaultSubobject<UFieldSystemComponent>(TEXT("Force System"));
	FieldSystemMetaDataFilter = CreateDefaultSubobject<UFieldSystemMetaDataFilter>(TEXT("FS Meta Data Filter"));
	RadialFalloff = CreateDefaultSubobject<URadialFalloff>(TEXT("Radial Falloff"));
	RadialVector = CreateDefaultSubobject<URadialVector>(TEXT("Radial Vector"));
}

void UDamageDealer::BeginPlay()
{
	Super::BeginPlay();	
}

void UDamageDealer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UDamageDealer::DealDamage(UPrimitiveComponent* PrimitiveComponent, FVector StartTracePos, FVector EndTracePos, float Damage, USoundBase* SoundBase)
{
	AActor* ActorOwner = GetOwner<AActor>();
	ActorsToIgnore.AddUnique(ActorOwner);
	FHitResult HitResult;
	BoxTrace(PrimitiveComponent, StartTracePos, EndTracePos, HitResult);
	const FVector ImpactPoint = HitResult.ImpactPoint;

	if(AActor* ActorHit = HitResult.GetActor())
	{
		ActorsToIgnore.AddUnique(ActorHit);

		if(SoundBase && Cast<APawn>(ActorHit))
		{
			UGameplayStatics::PlaySoundAtLocation(this, SoundBase, ImpactPoint, PrimitiveComponent -> GetComponentRotation());
			UGameplayStatics::SetGlobalTimeDilation(this, 0.01f);
			ActorOwner->GetWorldTimerManager().SetTimer(ImpactTimer, this, &UDamageDealer::ImpactPause, 0.001f);
		}

		UGameplayStatics::ApplyDamage(ActorHit,
			Damage,
			GetOwner()->GetInstigatorController(),
			ActorOwner,
			UDamageType::StaticClass()
		);
		
		if(IHitInterface* HitInterface = Cast<IHitInterface>(ActorHit))
		{
			HitInterface->Execute_GetHit(HitResult.GetActor(), ImpactPoint);			
		}

		CreateFields(ImpactPoint);
	}
}

void UDamageDealer::SetHitBoxCollisionType(TArray<UPrimitiveComponent*> PrimitiveComponents, ECollisionEnabled::Type CollisionType)
{
	if(PrimitiveComponents.Num() > 0)
	{
		for(UPrimitiveComponent* PrimitiveComponent: PrimitiveComponents)
		{
			PrimitiveComponent->SetCollisionEnabled(CollisionType);
		}

		if(CollisionType == ECollisionEnabled::NoCollision)
		{
			ActorsToIgnore.Empty();
		}
	}
}

void UDamageDealer::ImpactPause() const
{
	UGameplayStatics::SetGlobalTimeDilation(this, 1.f);
}

void UDamageDealer::BoxTrace(const UPrimitiveComponent* PrimitiveComponent, FVector StartTracePos, FVector EndTracePos, FHitResult& HitResult) const
{
	UKismetSystemLibrary::BoxTraceSingle(
	this,
	StartTracePos,
	EndTracePos,
	FVector(PrimitiveComponent->GetPlacementExtent().BoxExtent / 2),
	PrimitiveComponent->GetComponentRotation(),
	ETraceTypeQuery::TraceTypeQuery1,
	false,
	ActorsToIgnore,
	EDrawDebugTrace::None,
	HitResult,
	true
);
}