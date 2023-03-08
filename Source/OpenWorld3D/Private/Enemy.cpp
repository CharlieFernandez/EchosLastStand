// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "Components/AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	SkeletalMeshComponent = GetMesh();
	SkeletalMeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	SkeletalMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	SkeletalMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	SkeletalMeshComponent->SetGenerateOverlapEvents(true);
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	Attributes = CreateDefaultSubobject<UAttributeComponent>("Attribute Component");
	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>("Health Bar");
	HealthBarWidget->SetupAttachment(GetRootComponent());
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::GetHit_Implementation(const FVector ImpactPoint)
{
	UGameplayStatics::PlaySoundAtLocation(this, HitSFX, GetActorLocation(), GetActorRotation());
	EmitParticles(ImpactPoint);
	
	const double Angle = GetAngleFromImpactPoint(ImpactPoint);
	const FName SectionName = GenerateSectionName(Angle);	
	PlayReactMontage(SectionName);
}

double AEnemy::GetAngleFromImpactPoint(const FVector ImpactPoint)
{
	FVector Forward = GetActorForwardVector();
	const FVector ImpactLowered = FVector(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	FVector DirectionOfHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();
	
	double CosTheta = FVector::DotProduct(Forward, DirectionOfHit);
	double Theta = FMath::Acos(CosTheta);
	Theta = FMath::RadiansToDegrees(Theta);

	const FVector CrossProduct = FVector::CrossProduct(Forward, DirectionOfHit);

	if(CrossProduct.Z < 0)
	{
		Theta *= -1;
	}

	return Theta;
}

FName AEnemy::GenerateSectionName(double Angle)
{
	FName SectionName = FName("FromBack");

	if(Angle >= -45.f && Angle <= 45.f)
	{
		SectionName = FName("FromFront");
	}
	else if(Angle > 45.f && Angle < 135.f)
	{
		SectionName = FName("FromRight");
	}
	else if(Angle > -135.f && Angle < -45.f)
	{
		SectionName = FName("FromLeft");
	}

	return SectionName;
}

void AEnemy::PlayReactMontage(const FName& SectionName)
{
	if(UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->Montage_Play(ReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, ReactMontage);
	}
}

void AEnemy::EmitParticles(const FVector ImpactPoint)
{
	if(HitParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, HitParticles, ImpactPoint, FRotator::ZeroRotator, FVector::OneVector * 0.75f);
	}
}