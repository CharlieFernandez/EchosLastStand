// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterTypes.h"
#include "Characters/GameCharacter.h"
#include "GameFramework/Character.h"
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
	bool IsChasing() const;
	bool IsAttacking() const;
	virtual void Destroyed() override;

private:
	/* Fields */
	FTimerHandle PatrolTimerHandle;

	/* Methods */
	void StartWithWeapon();
	void CheckPatrolTarget();
	void CheckCombatTarget();
	bool IsInChasingRadius() const;
	bool IsInAttackingRadius() const;
	bool IsNearPatrolTarget() const;
	void SetPatrolTimer();


	virtual void Die() override;
	void SetStateToPatrolling();
	AActor* FindUniquePatrolTarget() const;
	void OnFinishedPatrolTimer() const;
	void SetStateToChasing(AActor* PawnToChase);
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;


	/* Properties */	
	UPROPERTY()
	TObjectPtr<AAIController> AIController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> StartingWeaponClass;

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

	/* Functions */
	UFUNCTION()
	void OnPawnSeen(APawn* PawnSeen);
};