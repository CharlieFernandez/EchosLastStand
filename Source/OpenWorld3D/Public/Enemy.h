// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterTypes.h"
#include "Characters/GameCharacter.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Enemy.generated.h"

class UPawnSensingComponent;
class AAIController;
class ATargetPoint;
class USphereComponent;
class UHealthBarComponent;
class UAttributeComponent;
class UAnimMontage;

UCLASS()
class OPENWORLD3D_API AEnemy : public AGameCharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	// Default Methods
	AEnemy();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Override Methods
	virtual void GetHit_Implementation(const FVector ImpactPoint) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	// Health Methods
	void ToggleHealth(bool Toggle) const;

	// Helper Methods
	void EmitHitParticles(FVector ImpactPoint) const;
	void DrawAllWaypoints();

protected:
	// Default Methods
	virtual void BeginPlay() override;

	// AI Navigation
	void GenerateNewPatrolTarget();
	void SetNewMoveToTarget(TObjectPtr<AActor> Target) const;

private:
	// Main Variables
	UPROPERTY(VisibleInstanceOnly)
	EEnemyState State;
	
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;

	// Core Methods
	void Die();
	
	// AI Navigation: Variables
	FTimerHandle PatrolTimerHandle;
	
	UPROPERTY()
	TObjectPtr<AAIController> AIController;
	
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<AActor> PatrolTarget;
	
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<TObjectPtr<AActor>> PatrolTargets;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float DistanceToGoalRadius;

	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<AActor> CombatTarget;

	UPROPERTY(EditDefaultsOnly)
	float ChasingDistance;

	UPROPERTY(EditDefaultsOnly)
	float AttackDistance;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPawnSensingComponent> PawnSensingComponent;

	// AI Navigation: Methods
	void SetStateToChasing(AActor* PawnToChase);
	void SetStateToPatrolling();
	void CheckPatrolTarget();
	void CheckCombatTarget();
	
	UFUNCTION()
	void OnPawnSeen(APawn* PawnSeen);
	void OnFinishedPatrolTimer() const;

	// Health 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAttributeComponent> Attributes;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UHealthBarComponent> HealthBarComponent;

	// Montages
	void PlayReactMontage(const FName& SectionName) const;
	
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage>ReactMontage;

	// Helper Methods
	double GetAngleFromImpactPoint(FVector ImpactPoint) const;	
	static FName GenerateSectionNameByAngle(double Angle);
};
