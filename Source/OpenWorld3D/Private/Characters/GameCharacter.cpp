// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/GameCharacter.h"

#include "Components/AttributeComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Items/Weapons/Weapon.h"

AGameCharacter::AGameCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SkeletalMeshComponent = GetMesh();
}

void AGameCharacter::BeginPlay()
{
	Super::BeginPlay();

	AnimInstance = GetMesh()->GetAnimInstance();
}

void AGameCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AGameCharacter::CanAttack()
{
	return false; // Change this later.
}

void AGameCharacter::Attack()
{
	if(CanAttack())
	{
		PlayComboAttackMontage();
	}
}

void AGameCharacter::PlayMontageSection(UAnimMontage* Montage, FName SectionName) const
{
	if(AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

void AGameCharacter::PlayComboAttackMontage()
{
	static uint8 AttackIndex = 0;
	
	ResetIfComboEnded(AttackIndex);
	
	if(NormalAttacksMontage->GetNumSections() <= AttackIndex) return;
	
	ActionState = EActionState::EAS_Attacking;
	PlayMontageSection(NormalAttacksMontage, NormalAttacksMontage->GetSectionName(AttackIndex));
	AttackIndex++;
}

void AGameCharacter::ResetIfComboEnded(uint8 &AttackIndex) const
{
	if(ActionState == EActionState::EAS_Unoccupied)
	{
		AttackIndex = 0;
	}
}

void AGameCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_AttackEnd;
}

void AGameCharacter::SetEquippedWeapon(AWeapon* Weapon, UMeshComponent* WeaponMesh, FName SN)
{
	AttachMeshToSocket(WeaponMesh, SN);	
	EquipState = EEquipState::ECS_Equipped;
	WeaponHeld = Weapon;
}

void AGameCharacter::Equip() const
{
	AttachMeshToSocket(GetWeaponHeld()->GetMesh(), rightHandItemSocket);	
}

void AGameCharacter::Unequip()
{
	AttachMeshToSocket(WeaponHeld->GetMesh(), spineWeaponSocket);
	EquipState = EEquipState::ECS_Unequipped;
	WeaponHeld = nullptr;
}

bool AGameCharacter::CanEquip() const
{
	return WeaponHeld &&
		EquipState == EEquipState::ECS_Unequipped &&
		ActionState != EActionState::EAS_Attacking;
}

bool AGameCharacter::CanUnequip() const
{
	return WeaponHeld &&
		EquipState != EEquipState::ECS_Unequipped &&
		ActionState != EActionState::EAS_Attacking;
}

void AGameCharacter::GetHit_Implementation(const FVector ImpactPoint)
{	
	EmitHitParticles(ImpactPoint);
	
	if(IsAlive())
	{
		FindAndPlayReactSection(ImpactPoint);
	}
	else Die();
}

bool AGameCharacter::IsAlive() const
{
	return Attributes->IsAlive();
}

void AGameCharacter::FindAndPlayReactSection(const FVector ImpactPoint) const
{
	const double Angle = GetAngleFromImpactPoint(ImpactPoint);
	const FName SectionName = GenerateSectionNameByAngle(Angle);	
	PlayReactMontage(SectionName);
}

float AGameCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	Attributes->UpdateHealth(-DamageAmount);
	
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void AGameCharacter::Die()
{
}

void AGameCharacter::AttachMeshToSocket(UMeshComponent* MeshToAttach, FName SocketName) const
{
	const FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
	MeshToAttach->AttachToComponent(GetMesh(), AttachmentTransformRules, SocketName);
}