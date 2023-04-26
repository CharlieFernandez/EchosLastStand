// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterTypes.h"
#include "Characters/GameCharacter.h"
#include "Enemy.generated.h"

class ULockableComponent;
class UNiagaraSystem;
class ASouls;
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

	inline const static FName EnemyTag = "Enemy";

protected:
	/* Methods */
	void GenerateNewPatrolTarget();
	virtual void BeginPlay() override;
	void SetNewMoveToTarget(TObjectPtr<AActor> Target) const;
	bool CanConsiderAttacking() const;
	virtual bool CanAttack() override;
	bool CanEngage() const;
	bool IsChasing() const;
	bool IsConsideringAttacking() const;
	bool IsEngaged() const;
	virtual void Destroyed() override;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<ULockableComponent> LockableComponent;

private:
	/* Fields */
	FTimerHandle PatrolTimerHandle;
	FTimerHandle AttackTimerHandle;

	/* Methods */
	void StartWithWeapon();
	void CheckPatrolTarget();
	void CheckCombatTarget();
	bool IsInChasingRadius() const;
	bool IsInAttackingRadius() const;
	bool IsNearPatrolTarget() const;
	void SetPatrolTimer();
	void ClearPatrolTimer();
	void StartAttackTimer();
	void ClearAttackTimer();

	bool IsDead() const;
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

	UPROPERTY(EditDefaultsOnly)
	float AttackMinTimer;

	UPROPERTY(EditDefaultsOnly)
	float AttackMaxTimer;
	
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

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ASouls> SoulsClass;
	
	/* Functions */
	UFUNCTION()
	void OnPawnSeen(APawn* PawnSeen);
};