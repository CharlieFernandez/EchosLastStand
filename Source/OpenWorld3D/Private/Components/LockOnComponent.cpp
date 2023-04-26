// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/LockOnComponent.h"

#include "Enemy.h"
#include "NiagaraComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/LockableComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ULockOnComponent::ULockOnComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	MidPointLockOn = CreateDefaultSubobject<USceneComponent>(TEXT("Target MidPoint"));
}

void ULockOnComponent::InitializeVariables() const
{	
	MidPointLockOn->SetupAttachment(GetOwner()->GetRootComponent());
}

void ULockOnComponent::InitializeVariablesOnBeginPlay(float DefaultDistance)
{
	DefaultCameraDistance = DefaultDistance;
}

void ULockOnComponent::BeginPlay()
{
	Super::BeginPlay();
	LockOnRadius = MaxCameraDistance;
	SpringArmComponent = Cast<USpringArmComponent>(GetOwner()->GetComponentByClass(USpringArmComponent::StaticClass()));
	CameraComponent = Cast<UCameraComponent>(GetOwner()->GetComponentByClass(UCameraComponent::StaticClass()));
	PlayerController = Cast<APlayerController>(GetOwner()->GetInstigatorController());	
	SetTickGroup(ETickingGroup::TG_EndPhysics);
}

void ULockOnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if(LockedOnTarget)
	{
		const AActor* Owner = GetOwner();
		MidPointLockOn->SetWorldLocation((Owner->GetActorLocation() + LockedOnTarget->GetActorLocation()) / 2);

		const float SpringArmLength = FVector::Distance(Owner->GetActorLocation(), LockedOnTarget->GetActorLocation());

		if(SpringArmLength > MaxCameraDistance)
		{
			Unlock();
		} else
		{
			SpringArmComponent->TargetArmLength = FMath::Clamp(SpringArmLength, MinCameraDistance, MaxCameraDistance);
		}
		
	}
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
	MidPointLockOn->SetWorldLocation((GetOwner()->GetActorLocation() + LockedOnTarget->GetActorLocation()) / 2);
	SpringArmComponent->AttachToComponent(MidPointLockOn, FAttachmentTransformRules::KeepRelativeTransform);
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
	if(LockedOnTarget == nullptr) return;
	
	CurrentlyUsedLockOnNC->DeactivateImmediate();
	LockedOnTarget = nullptr;
	SpringArmComponent->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	SpringArmComponent->TargetArmLength = DefaultCameraDistance;
}