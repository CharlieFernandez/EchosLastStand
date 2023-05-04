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
	FORCEINLINE EEquipState GetCharacterState() const { return EquipState; }
	FORCEINLINE UAttributeComponent* GetAttributes() const { return Attributes; }
	FORCEINLINE AWeapon* GetWeaponHeld() const { return WeaponHeld; }

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
	void PlayComboAttackMontage(UAnimMontage* AttackMontage);	
	virtual void BeginPlay() override;
	void SetEquippedWeapon(AWeapon* Weapon, UMeshComponent* WeaponMesh, FName SN);
	void AttachMeshToSocket(UMeshComponent* WeaponMesh, FName SocketName) const;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	FORCEINLINE float GetMaxSprintSpeed() const { return MaxSprintSpeed; }
	FORCEINLINE float GetMaxRunSpeed() const { return MaxRunSpeed; }
	FORCEINLINE float GetMaxWalkSpeed() const { return MaxWalkSpeed; }
	void ResetIfComboEnded(uint8& AttackIndex) const;
	FORCEINLINE UAnimMontage* GetSwordAttackMontage() const { return SwordAttacksMontage; }
	FORCEINLINE UAnimMontage* GetHammerAttackMontage() const { return HammerAttacksMontage; }
	void PlayMontageSection(UAnimMontage* Montage, FName SectionName) const;
	void SetHealthPercentage() const;
	bool IsUnoccupied() const;
	bool IsDashing() const;
	bool IsAttacking() const;
	bool IsAttackEnding() const;
	virtual void GetHit_Implementation(const FVector ImpactPoint, const FVector InstigatorPosition) override;

	/* Properties */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<AActor> CombatTarget;
	
	UPROPERTY(VisibleInstanceOnly)
	EEquipState EquipState = EEquipState::ECS_Unequipped;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UAttributeComponent> Attributes;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UHealthBarComponent> HealthBarComponent;
	
	UPROPERTY(EditDefaultsOnly, Category = Particles)
	TObjectPtr<UParticleSystem> HitParticles;
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimInstance> AnimInstance;
	
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	/* Functions */
	UFUNCTION(BlueprintCallable)
	void AttackEnd();
	UFUNCTION(BlueprintCallable)
	virtual void ResetActionState();

	UFUNCTION(BlueprintCallable)
	void Equip() const;

	UFUNCTION(BlueprintCallable)
	void Unequip();

	UFUNCTION(BlueprintCallable)
	void EmitParticles(FVector SpawnLocation, UParticleSystem* ParticleSystem) const;

private:	
	/* Methods */
	static FName GenerateSectionNameByAngle(double Angle);
	double GetAngleFromInstigatorPosition(FVector InstigatorPosition) const;
	void FindAndPlayReactSection(const FVector InstigatorPosition) const;

	/* Properties */	
	UPROPERTY(EditDefaultsOnly)
	bool CanBeStaggered = true;

	UPROPERTY(EditDefaultsOnly)
	float WarpTranslationOffset = 0.75f;
	
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> SwordAttacksMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> HammerAttacksMontage;
	
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

	/* Functions */
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	FVector GetTranslationWarp() const;

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	FVector GetRotationWarp() const;
};
