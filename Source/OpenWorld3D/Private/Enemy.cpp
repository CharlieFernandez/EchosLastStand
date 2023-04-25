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
#include "Characters/OpenWorldCharacter/OpenWorldCharacter.h"
#include "Components/LockableComponent.h"
#include "Items/Souls.h"
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
	
	HealthBarComponent = CreateDefaultSubobject<UHealthBarComponent>("Health Bar");
	HealthBarComponent->SetupAttachment(GetRootComponent());

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>("Pawn Sensor");
	LockableComponent = CreateDefaultSubobject<ULockableComponent>("Lockable");

	Tags.AddUnique("Enemy");
}

void AEnemy::BeginPlay()
{
	AIController = Cast<AAIController>(GetController());
	Super::BeginPlay();
	StartWithWeapon();
	State = EEnemyState::EES_Patrolling;
	ToggleHealth(false);
	// DrawAllWaypoints();
	GenerateNewPatrolTarget();
	OnFinishedPatrolTimer(); //Starts moving to the next waypoint.

	if(PawnSensingComponent)
	{
		PawnSensingComponent->OnSeePawn.AddDynamic(this, &AEnemy::OnPawnSeen);
	}
}

void AEnemy::StartWithWeapon()
{
	UWorld* World = GetWorld();
	
	if(StartingWeaponClass && World)
	{
		AWeapon* Weapon = Cast<AWeapon>(World->SpawnActor(StartingWeaponClass));
		SetEquippedWeapon(Weapon, Weapon->GetMesh(), rightHandItemSocket);
		Weapon->SetOwner(this);
		Weapon->SetInstigator(this);
		Weapon->ToggleWeaponState();
		Weapon->SetToHeldItem();
		Equip();
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(IsDead()) return;

	if(State > EEnemyState::EES_Patrolling)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}
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
	SetHealthPercentage();
	SetStateToChasing(EventInstigator->GetPawn());
	return DamageAmount;
}

void AGameCharacter::SetHealthPercentage() const
{
	if(HealthBarComponent)
	{		
		HealthBarComponent->SetHealthPercentage(Attributes->GetCurrentHealthPercent());
	}
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimerHandle);
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimerHandle);
}

void AEnemy::SetStateToChasing(AActor* PawnToChase)
{
	if(State == EEnemyState::EES_Dead) return;
	
	CombatTarget = PawnToChase;
	State = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = GetMaxRunSpeed();
	SetNewMoveToTarget(PawnToChase);
	ClearPatrolTimer();
}

void AEnemy::SetStateToPatrolling()
{
	if(State == EEnemyState::EES_Dead) return;
	
	CombatTarget = nullptr;
	SetNewMoveToTarget(PatrolTarget);
	State = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = GetMaxWalkSpeed();
}

void AEnemy::CheckCombatTarget()
{
	if(!IsInChasingRadius())
	{
		ClearAttackTimer();

		if(!IsEngaged())
		{
			SetStateToPatrolling();
		}
	}
	else if(!IsInAttackingRadius() && !IsChasing())
	{
		ClearAttackTimer();
		SetStateToChasing(CombatTarget);
	}
	else if(IsAttackEnding() && IsInAttackingRadius())
	{
		Attack();
	}
	else if(CanConsiderAttacking())
	{
		ClearAttackTimer();
		StartAttackTimer();
	}
}

bool AEnemy::CanConsiderAttacking() const
{
	return IsInAttackingRadius() &&
		!IsConsideringAttacking() &&
		!IsDead();
}

bool AEnemy::CanAttack()
{
	if(Super::CanAttack())
	{
		return IsInAttackingRadius() &&
			!IsAttacking() &&
			!IsDead();
	}

	return false;
}

bool AEnemy::CanEngage() const
{
	return IsInAttackingRadius() &&
		!IsEngaged() &&
		!IsDead();
}

void AEnemy::StartAttackTimer()
{	
	State = EEnemyState::EES_ConsideringAttack;
	const float AttackTime = FMath::RandRange(AttackMinTimer, AttackMaxTimer);
	GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AEnemy::Attack, AttackTime);
}

bool AEnemy::IsInChasingRadius() const
{
	return MyUtilities::InTargetRange(ChasingDistance, this, CombatTarget);
}

bool AEnemy::IsInAttackingRadius() const
{
	return MyUtilities::InTargetRange(AttackDistance, this, CombatTarget);
}

void AEnemy::CheckPatrolTarget()
{
	if(PatrolTarget && IsNearPatrolTarget())
	{
		GenerateNewPatrolTarget();
		SetPatrolTimer();
	}
}

void AEnemy::SetPatrolTimer()
{
	GetWorldTimerManager().SetTimer(PatrolTimerHandle, this, &AEnemy::OnFinishedPatrolTimer, 3.f);
}

bool AEnemy::IsNearPatrolTarget() const
{
	return MyUtilities::InTargetRange(DistanceToGoalRadius, this, PatrolTarget);
}

void AEnemy::ToggleHealth(bool Toggle) const
{
	HealthBarComponent->SetVisibility(Toggle);
}

void AGameCharacter::EmitParticles(FVector SpawnLocation, UParticleSystem* ParticleSystem) const
{
	if(ParticleSystem)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ParticleSystem, SpawnLocation, FRotator::ZeroRotator, FVector::OneVector * 0.75f);
	}
}

void AEnemy::Die()
{
	// Death animations are handled on the blueprint side.	
	if(State == EEnemyState::EES_Dead) return;
	
	Super::Die();
	
	State = EEnemyState::EES_Dead;
	CombatTarget = nullptr;
	ClearPatrolTimer();
	ClearAttackTimer();
	SetLifeSpan(10.f);
	ToggleHealth(false);
	AIController->StopMovement();

	if(UWorld* World = GetWorld())
	{
		ASouls* SpawnedSouls = World->SpawnActor<ASouls>(SoulsClass, FVector(1,1,1) * 10000000, GetActorRotation());

		if(SpawnedSouls)
		{
			SpawnedSouls->SetSoulCount(Attributes->GetTotalSouls());
			SpawnedSouls->SetActorLocation(GetActorLocation());
		}
	}
}

void AEnemy::Destroyed()
{
	if(GetWeaponHeld())
	{
		GetWeaponHeld()->Destroy();		
	}
}

/* Helper Methods */
bool AEnemy::IsDead() const
{
	return State == EEnemyState::EES_Dead;
}

bool AEnemy::IsChasing() const
{
	return State == EEnemyState::EES_Chasing;
}

bool AEnemy::IsConsideringAttacking() const
{
	return State == EEnemyState::EES_ConsideringAttack;
}

bool AEnemy::IsEngaged() const
{
	return State == EEnemyState::EES_Engaged;
}

void AEnemy::GenerateNewPatrolTarget()
{
	const uint8 NumOfPatrolTargets = PatrolTargets.Num();
	
	if(NumOfPatrolTargets > 0)
	{
		PatrolTarget = FindUniquePatrolTarget();
	}
}

AActor* AEnemy::FindUniquePatrolTarget() const
{
	TObjectPtr<AActor> NewTarget;
	
	do
	{
		const uint8 TargetIndex = FMath::RandRange(0, PatrolTargets.Num() - 1);
		NewTarget = PatrolTargets[TargetIndex];		
	} while(&PatrolTarget == &NewTarget);

	return NewTarget;
}

void AEnemy::SetNewMoveToTarget(TObjectPtr<AActor> Target) const
{
	if(AIController && Target)
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(15.f);
		AIController->MoveTo(MoveRequest);
	}
}

void AEnemy::DrawAllWaypoints()
{
	for(auto const Target: PatrolTargets)
	{
		FVector Location = Target->GetActorLocation();
		DrawDebugSphere(GetWorld(), Location, 12.f, 12, FColor::Emerald, true, -1.f);
	}
}