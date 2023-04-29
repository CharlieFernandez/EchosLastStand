// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/LockOnComponent.h"

#include "Enemy.h"
#include "NiagaraComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/LockableComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ULockOnComponent::ULockOnComponent()
{
	PrimaryComponentTick.bCanEverTick = true;	
}

void ULockOnComponent::InitializeVariables()
{

}

void ULockOnComponent::InitializeVariablesOnBeginPlay(float DefaultDistance)
{
	DefaultCameraDistance = DefaultDistance;
	MidPointLockOn = NewObject<USceneComponent>();
	MidPointSphereCollider = NewObject<USphereComponent>();
	MidPointSphereCollider->Deactivate();
	MidPointSphereCollider->SetSphereRadius(50);
	MidPointSphereCollider->SetUseCCD(true);
	MidPointSphereCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	MidPointSphereCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	
	MidPointLockOn->SetupAttachment(GetOwner()->GetRootComponent());
	MidPointSphereCollider->SetupAttachment(MidPointLockOn);
}

void ULockOnComponent::BeginPlay()
{
	Super::BeginPlay();
	SpringArmComponent = Cast<USpringArmComponent>(GetOwner()->GetComponentByClass(USpringArmComponent::StaticClass()));
	CameraComponent = Cast<UCameraComponent>(GetOwner()->GetComponentByClass(UCameraComponent::StaticClass()));
	PlayerController = Cast<APlayerController>(GetOwner()->GetInstigatorController());	
	SetTickGroup(ETickingGroup::TG_EndPhysics);
}

void ULockOnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	constexpr float Alpha = 0.05f;
	
	if(LockedOnTarget)
	{
		const AActor* Owner = GetOwner();
		const FVector PlayerLocation = Owner->GetActorLocation();
		const FVector EnemyLocation = LockedOnTarget->GetActorLocation();

		const FVector CameraToPlayerNormalized = (PlayerLocation - (CameraComponent->GetComponentLocation())).GetSafeNormal();
		const FVector PlayerToEnemyNormalized = (EnemyLocation - PlayerLocation).GetSafeNormal();
		const float Dot = FVector::DotProduct(CameraToPlayerNormalized, PlayerToEnemyNormalized);
		const float InvertedDot = 1 - FMath::Abs(Dot);
		const float OffsetMultiplier = Dot <= 0 ? 1 : InvertedDot;

		const float DistanceBetweenPlayerAndEnemy = FVector::Distance(PlayerLocation, EnemyLocation);
		
		float SpringArmLength = DistanceBetweenPlayerAndEnemy * OffsetMultiplier * 0.5 + SpringArmExtraSlack;
		
		SpringArmLength = FMath::Clamp(SpringArmLength, MinCameraDistance, MaxDistanceFromLockedTarget);
		SpringArmComponent->TargetArmLength = FMath::Lerp(SpringArmComponent->TargetArmLength, SpringArmLength, Alpha);

		const float CameraBackModifier = FVector::Distance(PlayerLocation,CameraComponent->GetComponentLocation() - SpringArmComponent->TargetOffset) / SpringArmComponent->TargetArmLength;
		const FVector SpringTargetOffset = PlayerToEnemyNormalized * FMath::Abs(DistanceBetweenPlayerAndEnemy) * 0.5f * OffsetMultiplier * CameraBackModifier;
		
		SpringArmComponent->TargetOffset = FMath::Lerp(SpringArmComponent->TargetOffset, SpringTargetOffset, Alpha);

		if(FVector::Distance(PlayerLocation, EnemyLocation) > MaxDistanceFromLockedTarget)
		{
			Unlock();
		}
	}
	else
	{
		SpringArmComponent->TargetArmLength = FMath::Lerp(SpringArmComponent->TargetArmLength, DefaultCameraDistance, Alpha);
		SpringArmComponent->TargetOffset = FMath::Lerp(SpringArmComponent->TargetOffset, FVector::Zero(), Alpha);
	}
}

void ULockOnComponent::Lock()
{	
	const TArray<AEnemy*> TargetsFound = FindAndFilterEnemies();

	if(TargetsFound.Num() == 0) return;
	MidPointLockOn->SetWorldLocation(GetOwner()->GetActorLocation());
	SetLockOnTarget(TargetsFound);
	const ULockableComponent* LockableComponent = Cast<ULockableComponent>(LockedOnTarget->GetComponentByClass(ULockableComponent::StaticClass()));
	CurrentlyUsedLockOnNC = LockableComponent->ActivateLockOnNS();
	//SpringArmComponent->AttachToComponent(MidPointLockOn, FAttachmentTransformRules::KeepRelativeTransform);
	MidPointSphereCollider->Activate();
}

void ULockOnComponent::Unlock()
{
	if(LockedOnTarget == nullptr) return;
	
	CurrentlyUsedLockOnNC->DeactivateImmediate();
	LockedOnTarget = nullptr;
	SpringArmComponent->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	MidPointSphereCollider->Deactivate();
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