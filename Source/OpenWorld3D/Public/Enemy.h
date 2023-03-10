// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/GameCharacter.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Enemy.generated.h"

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
	AEnemy();


	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void EmitHitParticles(FVector ImpactPoint) const;
	virtual void GetHit_Implementation(const FVector ImpactPoint) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	void ToggleHealth(bool Toggle);

protected:
	virtual void BeginPlay() override;
	void GenerateNewPatrolTarget();
	void SetNewMoveToTarget(TObjectPtr<AActor> Target) const;

private:
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;

	void PlayReactMontage(const FName& SectionName) const;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage>ReactMontage;

	double GetAngleFromImpactPoint(FVector ImpactPoint) const;
	FName GenerateSectionNameByAngle(double Angle) const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAttributeComponent> Attributes;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UHealthBarComponent> HealthBarComponent;

	void Die();

	UPROPERTY()
	TObjectPtr<AAIController> AIController;

	UPROPERTY()
	TObjectPtr<AActor> PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<TObjectPtr<AActor>> PatrolTargets;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolRadius;

	FTimerHandle TimerHandle;
	void OnFinishedPatrolTimer();
};
