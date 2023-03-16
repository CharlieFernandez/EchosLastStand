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

void AGameCharacter::Sprint()
{
	CharacterMovementComponent->MaxWalkSpeed = MaxSprintSpeed;
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
	static TArray<FName> AttacksNotUsed = ComboAttackMontageSectionNames;

	if(ActionState == EActionState::EAS_Unoccupied)
	{
		AttacksNotUsed = ComboAttackMontageSectionNames;
	}

	if(AttacksNotUsed.Num() <= 0)	return;

	ActionState = EActionState::EAS_Attacking;
	const int32 AttackSelection = FMath::RandRange(0, AttacksNotUsed.Num()-1);
	const FName AttackNameToUse = AttacksNotUsed[AttackSelection];
	AttacksNotUsed.RemoveAt(AttackSelection);
	
	PlayMontageSection(AttackMontage, AttackNameToUse);
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
	Weapon->SetOwner(this);
	Weapon->SetInstigator(this);
}

void AGameCharacter::Equip() const
{
	AttachMeshToSocket(WeaponHeld->GetMesh(), rightHandItemSocket);
}

void AGameCharacter::Unequip() const
{
	AttachMeshToSocket(WeaponHeld->GetMesh(), "spineWeaponSocket");
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

	if(Attributes)
	{
		if(Attributes->IsAlive())
		{
			const double Angle = GetAngleFromImpactPoint(ImpactPoint);
			const FName SectionName = GenerateSectionNameByAngle(Angle);	
			PlayReactMontage(SectionName);
		}
		else
		{
			Die();
		}
	}
}

float AGameCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
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