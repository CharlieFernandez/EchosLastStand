// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "AIController.h"
#include "MyUtilities.h"
#include "Components/AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/HealthBar.h"
#include "HUD/HealthBarComponent.h"
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
	HealthBarComponent = CreateDefaultSubobject<UHealthBarComponent>("Health Bar");
	HealthBarComponent->SetupAttachment(GetRootComponent());

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	ToggleHealth(false);

	AIController = Cast<AAIController>(GetController());

	for(auto const Target: PatrolTargets)
	{
		FVector Location = Target->GetActorLocation();
		DrawDebugSphere(GetWorld(), Location, 12.f, 12, FColor::Cyan, true, -1.f);
	}

	GenerateNewPatrolTarget();
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AEnemy::OnFinishedPatrolTimer, 1.f);
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(PatrolTarget && MyUtilities::InTargetRange(PatrolRadius, this, PatrolTarget))
	{
		GenerateNewPatrolTarget();
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AEnemy::OnFinishedPatrolTimer, 3.f);
	}
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if(Attributes && HealthBarComponent)
	{
		Attributes->UpdateHealth(-DamageAmount);
		HealthBarComponent->SetHealthPercentage(Attributes->GetCurrentHealthPercent());
	}

	return DamageAmount;
}

void AEnemy::GenerateNewPatrolTarget()
{
	TObjectPtr<AActor> NewTarget;
	
	do
	{
		const uint8 TargetIndex = FMath::RandRange(0, PatrolTargets.Num() - 1);
		NewTarget = PatrolTargets[TargetIndex];		
	} while(&PatrolTarget == &NewTarget);

	PatrolTarget = NewTarget;
}

void AEnemy::SetNewMoveToTarget(TObjectPtr<AActor> Target) const
{
	if(AIController && Target)
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(15.f);
		FNavPathSharedPtr NavPathSharedPtr;
		AIController->MoveTo(MoveRequest);
	}
}

void AEnemy::ToggleHealth(bool Toggle)
{
	HealthBarComponent->SetVisibility(Toggle);
}

void AEnemy::GetHit_Implementation(const FVector ImpactPoint)
{
	if(HitSFX)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSFX, GetActorLocation(), GetActorRotation());
	}
	
	EmitHitParticles(ImpactPoint);

	if(Attributes)
	{
		if(Attributes->IsAlive())
		{
			const double Angle = GetAngleFromImpactPoint(ImpactPoint);
			const FName SectionName = GenerateSectionNameByAngle(Angle);	
			PlayReactMontage(SectionName);
		}
		else
		{
			Die();
		}
	}
}

double AEnemy::GetAngleFromImpactPoint(const FVector ImpactPoint) const
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

FName AEnemy::GenerateSectionNameByAngle(double Angle) const
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

void AEnemy::PlayReactMontage(const FName& SectionName) const
{
	if(UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->Montage_Play(ReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, ReactMontage);
	}
}

void AEnemy::EmitHitParticles(const FVector ImpactPoint) const
{
	if(HitParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, HitParticles, ImpactPoint, FRotator::ZeroRotator, FVector::OneVector * 0.75f);
	}
}

// Death animations are handled on the blueprint side.
void AEnemy::Die()
{
	SetLifeSpan(10.f);
	ToggleHealth(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::OnFinishedPatrolTimer()
{
	if(PatrolTargets.Num() > 0 && AIController)
	{		
		SetNewMoveToTarget(PatrolTarget);
	}
}
