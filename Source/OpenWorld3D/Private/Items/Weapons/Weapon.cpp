// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Weapons/Weapon.h"
#include "Characters/OpenWorldCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Interfaces/HitInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"

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
		FVector(BoxComponent->GetUnscaledBoxExtent() / 2),
		BoxTraceStart->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		HitResult,
		true
	);

	const FVector ImpactPoint = HitResult.ImpactPoint;

	if(AActor* ActorHit = HitResult.GetActor())
	{
		ActorsToIgnore.AddUnique(ActorHit);

		if(HitSFX && Cast<APawn>(ActorHit))
		{
			UGameplayStatics::PlaySoundAtLocation(this, HitSFX, ImpactPoint, GetActorRotation());

			UGameplayStatics::SetGlobalTimeDilation(this, 0.01f);
			GetWorldTimerManager().SetTimer(WeaponHitPauseTimer, this, &AWeapon::SlightPause, 0.001f);
		}

		UGameplayStatics::ApplyDamage(ActorHit,
			Damage,
			GetInstigator()->GetController(),
			this,
			UDamageType::StaticClass()
		);
		
		if(IHitInterface* HitInterface = Cast<IHitInterface>(ActorHit))
		{
			HitInterface->Execute_GetHit(HitResult.GetActor(), ImpactPoint);			
		}

		CreateFields(ImpactPoint);
	}
}

void AWeapon::ToggleWeaponState()
{
	ItemState = (ItemState == EItemState::EIS_Hovering) ? EItemState::EIS_Held : EItemState::EIS_Hovering;

	if(ItemState == EItemState::EIS_Held)
	{		
		SphereComponent->SetGenerateOverlapEvents(false);
		PickUpParticles->Deactivate();
		ItemMesh->EmptyOverrideMaterials();
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

void AWeapon::SlightPause()
{
	UGameplayStatics::SetGlobalTimeDilation(this, 1.f);
}