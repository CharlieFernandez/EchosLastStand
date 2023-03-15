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
#include "Items/Weapons/Weapon.h"

/* Core Methods */
AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	
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

	UWorld* World = GetWorld();

	if(StartingWeaponClass && World)
	{
		AWeapon* Weapon = Cast<AWeapon>(World->SpawnActor(StartingWeaponClass));
		SetEquippedWeapon(Weapon, Weapon->GetMesh(), rightHandItemSocket);
		Weapon->SetToHeldItem();
		Equip();
	}

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

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	if(Attributes && HealthBarComponent)
	{
		Attributes->UpdateHealth(-DamageAmount);
		HealthBarComponent->SetHealthPercentage(Attributes->GetCurrentHealthPercent());
	}

	SetStateToChasing(EventInstigator->GetPawn());

	return DamageAmount;
}

void AEnemy::SetStateToChasing(AActor* PawnToChase)
{
	CombatTarget = PawnToChase;
	State = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = GetMaxRunSpeed();
	SetNewMoveToTarget(PawnToChase);
	GetWorldTimerManager().ClearTimer(PatrolTimerHandle);
}

void AEnemy::SetStateToPatrolling()
{
	CombatTarget = nullptr;
	SetNewMoveToTarget(PatrolTarget);
	State = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = GetMaxWalkSpeed();
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

void AEnemy::ToggleHealth(bool Toggle) const
{
	HealthBarComponent->SetVisibility(Toggle);
}

void AGameCharacter::PlayReactMontage(const FName& SectionName) const
{	
	if(AnimInstance)
	{
		AnimInstance->Montage_Play(ReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, ReactMontage);
	}
}

void AGameCharacter::EmitHitParticles(const FVector ImpactPoint) const
{
	if(HitParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, HitParticles, ImpactPoint, FRotator::ZeroRotator, FVector::OneVector * 0.75f);
	}
}

void AEnemy::Die()
{
	// Death animations are handled on the blueprint side.
	
	Super::Die();
	
	GetWorldTimerManager().ClearTimer(PatrolTimerHandle);
	SetLifeSpan(10.f);
	ToggleHealth(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::Destroyed()
{
	if(GetWeaponHeld())
	{
		GetWeaponHeld()->Destroy();		
	}
}

/* Helper Methods */
void AEnemy::GenerateNewPatrolTarget()
{
	TObjectPtr<AActor> NewTarget;

	const uint8 NumOfPatrolTargets = PatrolTargets.Num();
	if(NumOfPatrolTargets > 0)
	{
		do
		{
			const uint8 TargetIndex = FMath::RandRange(0, PatrolTargets.Num() - 1);
			NewTarget = PatrolTargets[TargetIndex];		
		} while(&PatrolTarget == &NewTarget);

		PatrolTarget = NewTarget;
	}
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

FName AGameCharacter::GenerateSectionNameByAngle(double Angle)
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

double AGameCharacter::GetAngleFromImpactPoint(const FVector ImpactPoint) const
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
		DrawDebugSphere(GetWorld(), Location, 12.f, 12, FColor::Emerald, true, -1.f);
	}
}