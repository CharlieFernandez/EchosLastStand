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
class OPENWORLD3D_API AEnemy : public AGameCharacter
{
	GENERATED_BODY()

public:
	/* Methods */
	AEnemy();
	void DrawAllWaypoints();
	void ToggleHealth(bool Toggle) const;	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;	

protected:
	/* Methods */
	void GenerateNewPatrolTarget();
	virtual void BeginPlay() override;
	void SetNewMoveToTarget(TObjectPtr<AActor> Target) const;

private:
	/* Fields */
	FTimerHandle PatrolTimerHandle;

	/* Methods */
	void SetStateToChasing(AActor* PawnToChase);
	void SetStateToPatrolling();
	void CheckPatrolTarget();
	void CheckCombatTarget();
	void OnFinishedPatrolTimer() const;
	virtual void Die() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	/* Properties */	
	UPROPERTY()
	TObjectPtr<AAIController> AIController;

	UPROPERTY(VisibleInstanceOnly)
	EEnemyState State;	
	
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<AActor> PatrolTarget;
	
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float DistanceToGoalRadius;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<TObjectPtr<AActor>> PatrolTargets;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPawnSensingComponent> PawnSensingComponent;
	
	UPROPERTY(EditDefaultsOnly)
	float AttackDistance;
	
	UPROPERTY(EditDefaultsOnly)
	float ChasingDistance;
	
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<AActor> CombatTarget;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UHealthBarComponent> HealthBarComponent;

	/* Functions */
	UFUNCTION()
	void OnPawnSeen(APawn* PawnSeen);
};