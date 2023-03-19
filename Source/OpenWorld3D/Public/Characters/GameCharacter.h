// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterTypes.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "GameCharacter.generated.h"

class UHealthBarComponent;
class AWeapon;
class UAttributeComponent;

UCLASS()
class OPENWORLD3D_API AGameCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	/* Fields */
	const inline static FName rightHandItemSocket = FName("rightHandWeaponSocket");
	const inline static FName spineWeaponSocket = FName("spineWeaponSocket");
	
	/* Methods */
	AGameCharacter();
	virtual void Tick(float DeltaTime) override;
	bool IsAlive() const;

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
	void PlayComboAttackMontage();	
	virtual void BeginPlay() override;
	void PlayReactMontage(const FName& SectionName) const;
	void SetEquippedWeapon(AWeapon* Weapon, UMeshComponent* WeaponMesh, FName SN);
	FORCEINLINE AWeapon* GetWeaponHeld() const { return WeaponHeld; }
	void AttachMeshToSocket(UMeshComponent* WeaponMesh, FName SocketName) const;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	FORCEINLINE float GetMaxSprintSpeed() const { return MaxSprintSpeed; }
	FORCEINLINE float GetMaxRunSpeed() const { return MaxRunSpeed; }
	FORCEINLINE float GetMaxWalkSpeed() const { return MaxWalkSpeed; }
	void ResetIfComboEnded(uint8& AttackIndex) const;
	FORCEINLINE UAnimMontage* GetAttackMontage() const { return NormalAttacksMontage; }
	void PlayMontageSection(UAnimMontage* Montage, FName SectionName) const;
	void SetHealthPercentage() const;
	bool IsUnoccupied() const;
	bool IsAttacking() const;



	/* Properties */
	UPROPERTY(VisibleInstanceOnly)
	EEquipState EquipState = EEquipState::ECS_Unequipped;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UAttributeComponent> Attributes;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UHealthBarComponent> HealthBarComponent;
	
	UPROPERTY(EditDefaultsOnly, Category = "Visual Effects")
	TObjectPtr<UParticleSystem> HitParticles;
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimInstance> AnimInstance;
	
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	/* Functions */
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void AttackEnd();

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void Equip() const;

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void Unequip();

private:
	/* Methods */
	void EmitHitParticles(FVector ImpactPoint) const;
	static FName GenerateSectionNameByAngle(double Angle);
	double GetAngleFromImpactPoint(FVector ImpactPoint) const;
	virtual void GetHit_Implementation(const FVector ImpactPoint) override;
	void FindAndPlayReactSection(const FVector ImpactPoint) const;

	/* Properties */	
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> NormalAttacksMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage>ReactMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MaxWalkSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MaxRunSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MaxSprintSpeed;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AWeapon> WeaponHeld;
};
