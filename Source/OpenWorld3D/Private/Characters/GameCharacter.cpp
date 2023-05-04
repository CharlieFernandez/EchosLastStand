// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/GameCharacter.h"

#include "Components/AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Items/Weapons/Weapon.h"

AGameCharacter::AGameCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SkeletalMeshComponent = GetMesh();
	Attributes = CreateDefaultSubobject<UAttributeComponent>("Attribute Component");
}

void AGameCharacter::BeginPlay()
{
	Super::BeginPlay();

	AnimInstance = GetMesh()->GetAnimInstance();
	SetInstigator(this);
}

void AGameCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AGameCharacter::CanAttack()
{
	return IsAlive() && (ActionState == EActionState::EAS_Unoccupied || ActionState == EActionState::EAS_AttackEnd)
	&& EquipState != EEquipState::ECS_Unequipped;
}

void AGameCharacter::Attack()
{
	if(CanAttack())
	{
		UAnimMontage* AttackMontage = WeaponHeld->GetWeaponType() == EWeaponType::EWT_Light ? SwordAttacksMontage : HammerAttacksMontage;
		PlayComboAttackMontage(AttackMontage);
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

void AGameCharacter::PlayComboAttackMontage(UAnimMontage* AttackMontage)
{
	if(AttackMontage == nullptr) return;
	
	static uint8 AttackIndex = 0;
	
	ResetIfComboEnded(AttackIndex);
	
	if(AttackMontage->GetNumSections() <= AttackIndex) return;
	
	ActionState = EActionState::EAS_Attacking;
	PlayMontageSection(AttackMontage, AttackMontage->GetSectionName(AttackIndex));
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

void AGameCharacter::ResetActionState()
{
	ActionState = EActionState::EAS_Unoccupied;
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

void AGameCharacter::GetHit_Implementation(const FVector ImpactPoint, const FVector InstigatorPosition)
{
	EmitParticles(ImpactPoint, HitParticles);
	
	if(IsAlive())
	{
		if(CanBeStaggered)
		{
			ActionState = EActionState::EAS_HitReact;
			FindAndPlayReactSection(InstigatorPosition);
		}
	}	
	else Die();
}

bool AGameCharacter::IsAlive() const
{
	return Attributes->IsAlive();
}

bool AGameCharacter::IsUnoccupied() const
{
	return ActionState == EActionState::EAS_Unoccupied;
}

bool AGameCharacter::IsDashing() const
{
	return ActionState == EActionState::EAS_Dashing;
}

bool AGameCharacter::IsAttacking() const
{
	return ActionState == EActionState::EAS_Attacking;
}

bool AGameCharacter::IsAttackEnding() const
{
	return ActionState == EActionState::EAS_AttackEnd;
}


void AGameCharacter::FindAndPlayReactSection(const FVector InstigatorPosition) const
{
	const double Angle = GetAngleFromInstigatorPosition(InstigatorPosition);
	const FName SectionName = GenerateSectionNameByAngle(Angle);	
	PlayMontageSection(ReactMontage, SectionName);
}

FName AGameCharacter::GenerateSectionNameByAngle(double Angle)
{
	FName SectionName = FName("FromBack");

	if(Angle >= -45.f && Angle <= 45.f)
	{
		SectionName = FName("FromFront");
	}
	else if(Angle > 45.f && Angle < 135.f)
	{
		SectionName = FName("FromRight");
	}
	else if(Angle > -135.f && Angle < -45.f)
	{
		SectionName = FName("FromLeft");
	}

	return SectionName;
}

double AGameCharacter::GetAngleFromInstigatorPosition(const FVector InstigatorPosition) const
{
	const FVector Forward = GetActorForwardVector();
	const FVector ImpactLowered = FVector(InstigatorPosition.X, InstigatorPosition.Y, GetActorLocation().Z);
	const FVector DirectionOfHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();
	
	const double CosTheta = FVector::DotProduct(Forward, DirectionOfHit);
	double Theta = FMath::Acos(CosTheta);
	Theta = FMath::RadiansToDegrees(Theta);

	const FVector CrossProduct = FVector::CrossProduct(Forward, DirectionOfHit);

	if(CrossProduct.Z < 0)
	{
		Theta *= -1;
	}

	return Theta;
}

float AGameCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	Attributes->UpdateHealth(-DamageAmount);
	
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void AGameCharacter::Die()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AGameCharacter::AttachMeshToSocket(UMeshComponent* MeshToAttach, FName SocketName) const
{
	const FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
	MeshToAttach->AttachToComponent(GetMesh(), AttachmentTransformRules, SocketName);
}

FVector AGameCharacter::GetTranslationWarp() const
{
	if(CombatTarget == nullptr) return FVector();

	const FVector Location = GetActorLocation();
	const FVector OtherLocation = CombatTarget->GetActorLocation();
	FVector VectorDirection = (Location - OtherLocation).GetSafeNormal();
	VectorDirection *= WarpTranslationOffset;

	return VectorDirection + OtherLocation;
}

FVector AGameCharacter::GetRotationWarp() const
{
	if(CombatTarget == nullptr) return GetActorLocation()+GetActorForwardVector() ;

	return CombatTarget->GetActorLocation();
}
