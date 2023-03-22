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

	for(FDamageHitBox DamageHitBox: DamageHitBoxes)
	{
		UPrimitiveComponent* HitBox = DamageHitBox.GetHitBox();
		HitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		HitBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		HitBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

		HitBox->OnComponentBeginOverlap.AddDynamic(this, &UDamageDealer::OnBoxBeginOverlap);
	}
}

void UDamageDealer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UDamageDealer::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const FDamageHitBox* DamageHitBox = DamageHitBoxes.FindByPredicate([OverlappedComponent](const FDamageHitBox DamageHitBoxTemp)
	{
		return DamageHitBoxTemp.GetHitBox() == OverlappedComponent;
	});

	const APawn* AttackersInstigator = GetOwner()->GetInstigator();
	const APawn* DefendersInstigator = OtherActor->GetInstigator();
	
	if(AttackersInstigator->ActorHasTag(TEXT("Enemy")) && DefendersInstigator->ActorHasTag(TEXT("Enemy")))
		return;
	
	DealDamage(
		OverlappedComponent,
		OverlappedComponent->GetChildComponent(0)->GetComponentLocation(),
		OverlappedComponent->GetChildComponent(1)->GetComponentLocation(),
		DamageHitBox->GetDamage(),
		DamageHitBox->GetHitSFX(),
		DamageHitBox->GetImpactPauseTime()
	);
}

void UDamageDealer::DealDamage(UPrimitiveComponent* PrimitiveComponent, FVector StartTracePos, FVector EndTracePos, float Damage, USoundBase* SoundBase, float ImpactPauseTime)
{
	AActor* ActorOwner = GetOwner<AActor>();
	ActorsToIgnore.AddUnique(ActorOwner);
	FHitResult HitResult;
	BoxTrace(PrimitiveComponent, StartTracePos, EndTracePos, HitResult);
	const FVector ImpactPoint = HitResult.ImpactPoint;

	UE_LOG(LogTemp, Warning, TEXT("Hit someone"));
	
	if(AActor* ActorHit = HitResult.GetActor())
	{
		ActorsToIgnore.AddUnique(ActorHit);

		if(SoundBase && Cast<APawn>(ActorHit))
		{
			UGameplayStatics::PlaySoundAtLocation(this, SoundBase, ImpactPoint, PrimitiveComponent -> GetComponentRotation());
			UGameplayStatics::SetGlobalTimeDilation(this, 0.01f);
			ActorOwner->GetWorldTimerManager().SetTimer(ImpactTimer, this, &UDamageDealer::ImpactPause, ImpactPauseTime);
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