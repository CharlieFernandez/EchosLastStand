// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterTypes.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "GameCharacter.generated.h"

class AWeapon;
class UAttributeComponent;

UCLASS()
class OPENWORLD3D_API AGameCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	/* Methods */
	void Sprint();
	AGameCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	/* Fields */
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;
	TObjectPtr<UCharacterMovementComponent> CharacterMovementComponent;

	/* Methods */
	void Attack();
	virtual void Die();
	bool CanEquip() const;
	bool CanUnequip() const;
	virtual bool CanAttack();
	void PlayAttackMontage() const;	
	virtual void BeginPlay() override;
	void PlayReactMontage(const FName& SectionName) const;
	void AttachMeshToSocket(UMeshComponent* WeaponMesh, FName SocketName) const;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	FORCEINLINE float GetMaxRunSpeed() const { return MaxRunSpeed; }
	FORCEINLINE float GetMaxWalkSpeed() const { return MaxWalkSpeed; }
	FORCEINLINE float GetMaxSprintSpeed() const { return MaxSprintSpeed; }
	FORCEINLINE UAnimMontage* GetAttackMontage() const { return AttackMontage; }

	/* Properties */
	UPROPERTY(VisibleInstanceOnly)
	EEquipState EquipState = EEquipState::ECS_Unequipped;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UAttributeComponent> Attributes;
	
	UPROPERTY(EditDefaultsOnly, Category = "Visual Effects")
	TObjectPtr<UParticleSystem> HitParticles;
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimInstance> AnimInstance;
	
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AWeapon> WeaponHeld;	

	/* Functions */
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void AttackEnd();

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void Equip() const;

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void Unequip() const;

private:
	/* Methods */
	void EmitHitParticles(FVector ImpactPoint) const;
	static FName GenerateSectionNameByAngle(double Angle);
	double GetAngleFromImpactPoint(FVector ImpactPoint) const;
	virtual void GetHit_Implementation(const FVector ImpactPoint) override;

	/* Properties */
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> AttackMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage>ReactMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MaxWalkSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MaxRunSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MaxSprintSpeed;
};
