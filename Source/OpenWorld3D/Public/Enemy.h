// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Enemy.generated.h"

class UWidgetComponent;
class UAttributeComponent;
class UAnimMontage;

UCLASS()
class OPENWORLD3D_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	AEnemy();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void EmitParticles(FVector ImpactPoint);
	virtual void GetHit_Implementation(const FVector ImpactPoint);

protected:
	virtual void BeginPlay() override;

private:
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;

	void PlayReactMontage(const FName& SectionName);

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage>ReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = Sounds)
	TObjectPtr<USoundBase> HitSFX;

	UPROPERTY(EditDefaultsOnly, Category = "Visual Effects")
	TObjectPtr<UParticleSystem> HitParticles;

	double GetAngleFromImpactPoint(FVector ImpactPoint);
	FName GenerateSectionName(double Angle);

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAttributeComponent> Attributes;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWidgetComponent> HealthBarWidget;
};
