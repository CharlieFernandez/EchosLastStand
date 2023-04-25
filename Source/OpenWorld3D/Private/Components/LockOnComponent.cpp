// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/LockOnComponent.h"

#include "Enemy.h"
#include "NiagaraComponent.h"
#include "Components/LockableComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ULockOnComponent::ULockOnComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void ULockOnComponent::BeginPlay()
{
	Super::BeginPlay();

	SetTickGroup(ETickingGroup::TG_PostPhysics);
}

void ULockOnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(LockedOnTarget)
	{				
		const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), LockedOnTarget->GetActorLocation());
		UGameplayStatics::GetPlayerController(this, 0)->SetControlRotation(LookAtRotation);	
	}
}

void ULockOnComponent::ToggleLockOntoTarget()
{
	if(LockedOnTarget) Unlock();
	else Lock();
}

void ULockOnComponent::Lock()
{
	TArray<AActor*> ActorsToIgnore;
	TArray<FHitResult> HitResults;
	ActorsToIgnore.AddUnique(GetOwner());
	FindAndFilterEnemies(ActorsToIgnore, HitResults);

	if(HitResults.Num() == 0) return;

	SetLockOnTarget(HitResults);
	const ULockableComponent* LockableComponent = Cast<ULockableComponent>(LockedOnTarget->GetComponentByClass(ULockableComponent::StaticClass()));
	CurrentlyUsedLockOnNC = LockableComponent->ActivateLockOnNS();
	SpringArmComponent->TargetOffset = LockOnOffset;
}

void ULockOnComponent::SetLockOnTarget(const TArray<FHitResult>& HitResults)
{
	const FVector ActorLocation = GetOwner()->GetActorLocation();
	LockedOnTarget = HitResults[0].GetActor();

	for(int i = 1; i < HitResults.Num(); i++)
	{
		const TObjectPtr<AActor> EnemyToInspect = HitResults[i].GetActor();
		const float CurrentClosestDistance =(LockedOnTarget->GetActorLocation() - ActorLocation).SquaredLength();
		const float NextDistance = (EnemyToInspect->GetActorLocation() - ActorLocation).SquaredLength();

		if(CurrentClosestDistance > NextDistance)
		{
			LockedOnTarget = EnemyToInspect;

			if(CurrentlyUsedLockOnNC)
			{
				CurrentlyUsedLockOnNC->Activate();
			}
		}
	}
}

void ULockOnComponent::FindAndFilterEnemies(TArray<AActor*> ActorsToIgnore, TArray<FHitResult>& HitResults) const
{
	const FVector ActorLocation = GetOwner()->GetActorLocation();
	
	UKismetSystemLibrary::SphereTraceMulti(this,
	ActorLocation,
	ActorLocation,
	LockOnRadius,
	ETraceTypeQuery::TraceTypeQuery1,
	false,
	ActorsToIgnore,
	EDrawDebugTrace::None,
	HitResults,
	true
);

	HitResults.RemoveAll([](FHitResult HitResult)
	{
		const AEnemy* Enemy = Cast<AEnemy>(HitResult.GetActor());
		
		return Enemy == nullptr || !Enemy->IsAlive();
	});
}

void ULockOnComponent::Unlock()
{
	CurrentlyUsedLockOnNC->DeactivateImmediate();
	LockedOnTarget = nullptr;
	SpringArmComponent->TargetOffset = FVector();
}