// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/LockOnComponent.h"

#include "Enemy.h"
#include "NiagaraComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/LockableComponent.h"
#include "Engine/RendererSettings.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ULockOnComponent::ULockOnComponent()
{
	PrimaryComponentTick.bCanEverTick = true;	
}

void ULockOnComponent::InitializeVariablesOnBeginPlay(float DefaultDistance)
{
	DefaultCameraDistance = DefaultDistance;
}

void ULockOnComponent::BeginPlay()
{
	Super::BeginPlay();
	SpringArmComponent = Cast<USpringArmComponent>(GetOwner()->GetComponentByClass(USpringArmComponent::StaticClass()));
	CameraComponent = Cast<UCameraComponent>(GetOwner()->GetComponentByClass(UCameraComponent::StaticClass()));
	SetTickGroup(ETickingGroup::TG_EndPhysics);
}

void ULockOnComponent::EvaluateVectorsForDotProduct()
{
	CameraToPlayerNormalized = (PlayerLocation - CameraComponent->GetComponentLocation()).GetSafeNormal();
	PlayerToEnemyNormalized = (EnemyLocation - PlayerLocation).GetSafeNormal();
}

void ULockOnComponent::EvaluateSpringArmLengthMultiplier()
{
	Dot = FVector::DotProduct(CameraToPlayerNormalized, PlayerToEnemyNormalized);
	InvertedDot = 1 - FMath::Abs(Dot);
	SpringArmLengthMultiplier = Dot <= 0 ? 1 : InvertedDot;
}

void ULockOnComponent::SetSpringArmLength()
{
	DistanceBetweenPlayerAndEnemy = FVector::Distance(PlayerLocation, EnemyLocation);		
	DesiredSpringArmLength = DistanceBetweenPlayerAndEnemy * SpringArmLengthMultiplier * 0.5 + SpringArmExtraSlack;
	DesiredSpringArmLength = FMath::Clamp(DesiredSpringArmLength, MinCameraDistance, MaxDistanceFromLockedTarget);
	SpringArmComponent->TargetArmLength = FMath::Lerp(SpringArmComponent->TargetArmLength, DesiredSpringArmLength, Alpha);
}

void ULockOnComponent::SetSpringArmTargetOffset()
{
	CameraBackModifier = FVector::Distance(PlayerLocation,CameraComponent->GetComponentLocation() - SpringArmComponent->TargetOffset) / SpringArmComponent->TargetArmLength;
	const FVector SpringTargetOffset = PlayerToEnemyNormalized * FMath::Abs(DistanceBetweenPlayerAndEnemy) * HalfDistanceMultiplierForCameraOffset * SpringArmLengthMultiplier * CameraBackModifier;
	SpringArmComponent->TargetOffset = FMath::Lerp(SpringArmComponent->TargetOffset, SpringTargetOffset, Alpha);
}

void ULockOnComponent::EvaluateSpringArmAndCameraProperties()
{
	EvaluateVectorsForDotProduct();
	EvaluateSpringArmLengthMultiplier();
	SetSpringArmLength();
	SetSpringArmTargetOffset();
}

void ULockOnComponent::AdjustCameraDuringLockOn()
{
	EvaluateSpringArmAndCameraProperties();
	
	if(FVector::Distance(PlayerLocation, EnemyLocation) > MaxDistanceFromLockedTarget)
	{
		Unlock();
	}
}

void ULockOnComponent::AdjustCameraWhenUnlocking() const
{
	SpringArmComponent->TargetArmLength = FMath::Lerp(SpringArmComponent->TargetArmLength, DefaultCameraDistance, Alpha);
	SpringArmComponent->TargetOffset = FMath::Lerp(SpringArmComponent->TargetOffset, FVector::Zero(), Alpha);
}

void ULockOnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if(LockedOnTarget)
	{
		PlayerLocation = GetOwner()->GetActorLocation();
		EnemyLocation = LockedOnTarget->GetActorLocation();
		AdjustCameraDuringLockOn();
	}
	else
	{
		AdjustCameraWhenUnlocking();
	}
}

void ULockOnComponent::ActivateTargetingVFX()
{
	const ULockableComponent* LockableComponent = Cast<ULockableComponent>(LockedOnTarget->GetComponentByClass(ULockableComponent::StaticClass()));
	CurrentlyUsedLockOnNC = LockableComponent->ActivateLockOnNS();
}

void ULockOnComponent::Lock()
{	
	const TArray<AEnemy*> TargetsFound = FindAndFilterEnemies();

	if(TargetsFound.Num() == 0) return;
	
	SetLockOnTarget(TargetsFound);
	ActivateTargetingVFX();
}

AEnemy* ULockOnComponent::FindEnemyToTheRight(TArray<AEnemy*> EnemiesFound, SwitchTargetDirection Direction)
{
	FVector2D PotentialTargetScreenPosition;
	FVector2D LockedOnTargetLocationRelativeToScreen;
	UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), LockedOnTarget->GetActorLocation(), LockedOnTargetLocationRelativeToScreen);

	AEnemy* PotentialEnemyToTarget = nullptr;
	float CurrentSmallestDistance = FLT_MAX;
	
	
	for(AEnemy* Enemy: EnemiesFound)
	{
		if(Enemy == LockedOnTarget) continue;

		UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), Enemy->GetActorLocation(), PotentialTargetScreenPosition);

		if(FMath::Abs(PotentialTargetScreenPosition.X - LockedOnTargetLocationRelativeToScreen.X) < CurrentSmallestDistance)
		{
			if(
				(Direction == SwitchTargetDirection::Right && LockedOnTargetLocationRelativeToScreen.X < PotentialTargetScreenPosition.X) ||
				(Direction == SwitchTargetDirection::Left && LockedOnTargetLocationRelativeToScreen.X > PotentialTargetScreenPosition.X)
			)
			{
				CurrentSmallestDistance = FMath::Abs(PotentialTargetScreenPosition.X - LockedOnTargetLocationRelativeToScreen.X);
				PotentialEnemyToTarget = Enemy;
			}	
		}
	}

	return PotentialEnemyToTarget;
}

AEnemy* ULockOnComponent::FindEnemyToTheLeft(TArray<AEnemy*> EnemiesFound, AEnemy*& PotentialEnemyToTarget, SwitchTargetDirection Direction)
{
	if(PotentialEnemyToTarget == nullptr)
	{
		FVector2D PotentialTargetScreenPosition;
		FVector2D LockedOnTargetLocationRelativeToScreen;
		UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), LockedOnTarget->GetActorLocation(), LockedOnTargetLocationRelativeToScreen);
		float MinScreenX = LockedOnTargetLocationRelativeToScreen.X;

		for(AEnemy* Enemy: EnemiesFound)
		{
			if(Enemy == LockedOnTarget) continue;

			if(UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), Enemy->GetActorLocation(), PotentialTargetScreenPosition))
			{
				if((Direction == SwitchTargetDirection::Left && PotentialTargetScreenPosition.X < MinScreenX ) ||
					(Direction == SwitchTargetDirection::Right && PotentialTargetScreenPosition.X > MinScreenX ))
				{
					MinScreenX = PotentialTargetScreenPosition.X;
					PotentialEnemyToTarget = Enemy;
				}
			}
		}
	}

	return PotentialEnemyToTarget;
}

void ULockOnComponent::ReplaceFormerTargetWithNewTarget(AEnemy* PotentialEnemyToTarget)
{
	if(PotentialEnemyToTarget)
	{
		LockedOnTarget = PotentialEnemyToTarget;
		CurrentlyUsedLockOnNC->DeactivateImmediate();
		
		const ULockableComponent* LockableComponent = Cast<ULockableComponent>(PotentialEnemyToTarget->GetComponentByClass(ULockableComponent::StaticClass()));
		CurrentlyUsedLockOnNC = LockableComponent->ActivateLockOnNS();
	}
}

void ULockOnComponent::SwitchTarget(SwitchTargetDirection Direction)
{
	if(LockedOnTarget == nullptr) return;
	
	const TArray<AEnemy*> EnemiesFound = FindAndFilterEnemies();	
	AEnemy* PotentialEnemyToTarget = FindEnemyToTheRight(EnemiesFound, Direction);
	const SwitchTargetDirection OppositeDirection = Direction == SwitchTargetDirection::Left ? Right : Left;
	PotentialEnemyToTarget = FindEnemyToTheLeft(EnemiesFound, PotentialEnemyToTarget,  OppositeDirection);
	ReplaceFormerTargetWithNewTarget(PotentialEnemyToTarget);
}

void ULockOnComponent::SwitchTargetRight()
{
	SwitchTarget(SwitchTargetDirection::Right);
}

void ULockOnComponent::SwitchTargetLeft()
{
	SwitchTarget(SwitchTargetDirection::Left);
}

void ULockOnComponent::Unlock()
{
	if(LockedOnTarget == nullptr) return;
	
	LockedOnTarget = nullptr;
	CurrentlyUsedLockOnNC->DeactivateImmediate();
}

TArray<AEnemy*> ULockOnComponent::FindAndFilterEnemies() const
{
	const FVector ActorLocation = GetOwner()->GetActorLocation();
	TArray<TEnumAsByte<EObjectTypeQuery>> traceObjectTypes;
	
	TArray<AActor*> TargetsFound;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.AddUnique(GetOwner());
	
	UKismetSystemLibrary::SphereOverlapActors(this, ActorLocation, LockOnRadius, traceObjectTypes, AEnemy::StaticClass(), ActorsToIgnore,  TargetsFound);

	TArray<AEnemy*> EnemiesFound;

	for (const auto Target: TargetsFound)
	{
		if(AEnemy* Enemy = Cast<AEnemy>(Target))
		{
			EnemiesFound.Add(Enemy);	
		}
	}
	
	traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));	
	EnemiesFound.RemoveAll([](const AEnemy* Enemy)
	{		
		return !Enemy->IsAlive();
	});

	return EnemiesFound;
}

void ULockOnComponent::SetLockOnTarget(const TArray<AEnemy*> Enemies)
{
	const FVector ActorLocation = GetOwner()->GetActorLocation();
	LockedOnTarget = Enemies[0];

	for(int i = 1; i < Enemies.Num(); i++)
	{
		const TObjectPtr<AEnemy> EnemyToInspect = Enemies[i];
		const float CurrentClosestDistance = FVector::Distance(LockedOnTarget->GetActorLocation(), ActorLocation);
		const float NextDistance = FVector::Distance(EnemyToInspect->GetActorLocation(), ActorLocation);

		if(CurrentClosestDistance > NextDistance)
		{
			LockedOnTarget = EnemyToInspect;
		}
	}
}