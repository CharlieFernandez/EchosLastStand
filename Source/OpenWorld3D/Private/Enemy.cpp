// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/CapsuleComponent.h"
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

void AEnemy::GetHit(const FVector ImpactPoint)
{	
	if(GetWorld())
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

		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Emerald, FString::Printf(TEXT("Theta: %f"), Theta));
		}

		UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + (CrossProduct * 60), 20, FLinearColor::Blue, 3, 2);
		UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + (Forward * 60), 20, FLinearColor::Green, 3, 2);
		UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + DirectionOfHit * 60, 20, FLinearColor::Red, 3, 2);
	}

	PlayReactMontage("FromFront");
}

void AEnemy::PlayReactMontage(const FName& SectionName)
{
	if(UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->Montage_Play(ReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, ReactMontage);
	}
}

