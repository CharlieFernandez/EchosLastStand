// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "AIController.h"
#include "MyUtilities.h"
#include "Components/AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Perception/PawnSensingComponent.h"
#include "Characters/CharacterTypes.h"
#include "Characters/OpenWorldCharacter.h"

// Default Methods
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

	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>("Pawn Sensor");
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	State = EEnemyState::EES_Patrolling;
	ToggleHealth(false);
	AIController = Cast<AAIController>(GetController());
	DrawAllWaypoints();
	GenerateNewPatrolTarget();
	GetWorldTimerManager().SetTimer(PatrolTimerHandle, this, &AEnemy::OnFinishedPatrolTimer, 1.f);

	if(PawnSensingComponent)
	{
		// PawnSensingComponent->OnSeePawn.AddDynamic(this, &AEnemy::OnPawnSeen);
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(State > EEnemyState::EES_Patrolling)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}	
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// Event & Callback Methods
void AEnemy::OnPawnSeen(APawn* PawnSeen)
{
	if(State > EEnemyState::EES_Patrolling) return;
	
	if(PawnSeen->ActorHasTag(AOpenWorldCharacter::GetPlayerTag()))
	{
		SetStateToChasing(PawnSeen);
	}
}

void AEnemy::OnFinishedPatrolTimer() const
{
	if(AIController)
	{		
		SetNewMoveToTarget(PatrolTarget);
	}
}

// Override Methods
void AEnemy::GetHit_Implementation(const FVector ImpactPoint)
{	
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

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if(Attributes && HealthBarComponent)
	{
		Attributes->UpdateHealth(-DamageAmount);
		HealthBarComponent->SetHealthPercentage(Attributes->GetCurrentHealthPercent());
	}

	SetStateToChasing(EventInstigator->GetPawn());

	return DamageAmount;
}

// AI
void AEnemy::SetStateToChasing(AActor* PawnToChase)
{
	CombatTarget = PawnToChase;
	State = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = MaxRunSpeed;
	SetNewMoveToTarget(PawnToChase);
	GetWorldTimerManager().ClearTimer(PatrolTimerHandle);
}

void AEnemy::SetStateToPatrolling()
{
	CombatTarget = nullptr;
	SetNewMoveToTarget(PatrolTarget);
	State = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}

void AEnemy::CheckCombatTarget()
{
	if(!MyUtilities::InTargetRange(ChasingDistance, this, CombatTarget))
	{
		SetStateToPatrolling();
	}
	else if(!MyUtilities::InTargetRange(AttackDistance, this, CombatTarget) && State != EEnemyState::EES_Chasing)
	{
		SetStateToChasing(CombatTarget);
	}
	else if(CombatTarget)
	{
		if(MyUtilities::InTargetRange(AttackDistance, this, CombatTarget) && State != EEnemyState::EES_Attacking)
		{
			State = EEnemyState::EES_Attacking;
		}	
	}
}

void AEnemy::CheckPatrolTarget()
{
	if(PatrolTarget && MyUtilities::InTargetRange(DistanceToGoalRadius, this, PatrolTarget))
	{
		GenerateNewPatrolTarget();
		GetWorldTimerManager().SetTimer(PatrolTimerHandle, this, &AEnemy::OnFinishedPatrolTimer, 3.f);
	}
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

// Health
void AEnemy::ToggleHealth(bool Toggle) const
{
	HealthBarComponent->SetVisibility(Toggle);
}

// Animation
void AEnemy::PlayReactMontage(const FName& SectionName) const
{
	if(AnimInstance)
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

// Core
/* Death animations are handled on the blueprint side.*/
void AEnemy::Die()
{
	GetWorldTimerManager().ClearTimer(PatrolTimerHandle);
	SetLifeSpan(10.f);
	ToggleHealth(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Helper Methods
FName AEnemy::GenerateSectionNameByAngle(double Angle)
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

double AEnemy::GetAngleFromImpactPoint(const FVector ImpactPoint) const
{
	const FVector Forward = GetActorForwardVector();
	const FVector ImpactLowered = FVector(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector DirectionOfHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();
	
	const double CosTheta = FVector::DotProduct(Forward, DirectionOfHit);
	double Theta = FMath::Acos(CosTheta);
	Theta = FMath::RadiansToDegrees(Theta);

	const FVector CrossProduct = FVector::CrossProduct(Forward, DirectionOfHit);

	if(CrossProduct.Z < 0)
	{
		Theta *= -1;
	}

	return Theta;
}

void AEnemy::DrawAllWaypoints()
{
	for(auto const Target: PatrolTargets)
	{
		FVector Location = Target->GetActorLocation();
		// DrawDebugSphere(GetWorld(), Location, 12.f, 12, FColor::Emerald, true, -1.f);
	}
}