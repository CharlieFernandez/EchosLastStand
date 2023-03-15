// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/OpenWorldCharacter.h"

#include "Enemy.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/SphereComponent.h"
#include "Items/Weapons/EquipActionState.h"
#include "Items/Weapons/Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AOpenWorldCharacter::AOpenWorldCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	CameraBoom = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraBoom->SetupAttachment(SpringArm);

	CharacterMovementComponent = GetCharacterMovement();

	CharacterMovementComponent -> bOrientRotationToMovement = true;
	CharacterMovementComponent -> RotationRate = FRotator(0.f, 400.f, 0.f);

	HealthRadiusSphereComponent = CreateDefaultSubobject<USphereComponent>("Combat Radius");
	HealthRadiusSphereComponent->SetupAttachment(GetRootComponent());
}

void AOpenWorldCharacter::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(PlayerTag);

	HealthRadiusSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AOpenWorldCharacter::OnSphereBeginOverlap);
	HealthRadiusSphereComponent->OnComponentEndOverlap.AddDynamic(this, &AOpenWorldCharacter::OnSphereEndOverlap);

	if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = PlayerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->AddMappingContext(OpenWorldCharacterContext, 0);
		}
	}
}

void AOpenWorldCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(CharacterMovementComponent->Velocity.Length() == 0)
	{
		CharacterMovementComponent->MaxWalkSpeed = GetMaxRunSpeed();
	}

	if(ActionState == EActionState::EAS_Rolling)
	{
		const FVector RollDisplacement = DirectionToRoll * RollSpeed * DeltaTime;

		AddMovementInput(GetActorForwardVector(), RollDisplacement.Length());
	}
}

void AOpenWorldCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent -> BindAction(MovementAction, ETriggerEvent::Triggered, this, &AOpenWorldCharacter::Move);
		EnhancedInputComponent -> BindAction(LookAroundAction, ETriggerEvent::Triggered, this, &AOpenWorldCharacter::LookAround);
		EnhancedInputComponent -> BindAction(JumpAction, ETriggerEvent::Triggered, this, &AOpenWorldCharacter::Jump);
		EnhancedInputComponent -> BindAction(EKeyPressedAction, ETriggerEvent::Triggered, this, &AOpenWorldCharacter::ObtainOrEquip);
		EnhancedInputComponent -> BindAction(AttackPressedAction, ETriggerEvent::Triggered, this, &AOpenWorldCharacter::Attack);
		EnhancedInputComponent -> BindAction(SprintAction, ETriggerEvent::Triggered, this, &AGameCharacter::Sprint);
		EnhancedInputComponent -> BindAction(RollAction, ETriggerEvent::Triggered, this, &AOpenWorldCharacter::Roll);
	}
}

void AOpenWorldCharacter::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(const AEnemy* Enemy = Cast<AEnemy>(OtherActor))
	{
		Enemy->ToggleHealth(true);
	}
}

void AOpenWorldCharacter::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) 
{
	if(const AEnemy* Enemy = Cast<AEnemy>(OtherActor))
	{
		Enemy->ToggleHealth(false);
	}
}

void AOpenWorldCharacter::Move(const FInputActionValue& Value)
{
	if(ActionState != EActionState::EAS_Unoccupied) return;
	
	const FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);
	
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void AOpenWorldCharacter::LookAround(const FInputActionValue& Value)
{
	const FVector2D RotationValue = Value.Get<FVector2D>();

	if(Controller && !RotationValue.IsZero())
	{
		AddControllerPitchInput(RotationValue.Y);
		AddControllerYawInput(RotationValue.X);
	}
}

void AOpenWorldCharacter::ObtainOrEquip()
{	
	if(	AWeapon* Weapon = Cast<AWeapon>(OverlappedItem))
	{
		PickUpWeapon(Weapon, Weapon->GetStaticMeshComponent(), rightHandItemSocket);
	}
	else
	{
		if(GetWeaponHeld() && ActionState == EActionState::EAS_Unoccupied)
		{
			if(CanUnequip())
			{			
				PlayEquipMontage(EEquipActionState::EEAS_Unequip);
				EquipState = EEquipState::ECS_Unequipped;
			}
			else if(CanEquip())
			{
				PlayEquipMontage(EEquipActionState::EEAS_Equip);
				EquipState = EEquipState::ECS_Equipped;
			}

			ActionState = EActionState::EAS_Equipping;
		}
	}
}

void AOpenWorldCharacter::PickUpWeapon(AWeapon* Weapon, UMeshComponent* WeaponMesh, FName SN)
{
	SetEquippedWeapon(Weapon, WeaponMesh, SN);
	Weapon->ToggleWeaponState();
	
	if(auto const PickUpSound = GetWeaponHeld()->GetPickUpSound())
	{
		UGameplayStatics::PlaySoundAtLocation(this, PickUpSound, Weapon->GetActorLocation());
	}
}

void AOpenWorldCharacter::PlayEquipMontage(EEquipActionState EquipType) const
{
	if(EquipMontage && GetWeaponHeld() && AnimInstance)
	{
		AnimInstance->Montage_Play(EquipMontage);
		const UEnum* EnumEquipActionState = StaticEnum<EEquipActionState>();
		const FString EnumString = EnumEquipActionState->GetDisplayNameTextByValue(static_cast<int64>(EquipType)).ToString();
		AnimInstance->Montage_JumpToSection(FName(*EnumString), EquipMontage);
	}	
}

void AOpenWorldCharacter::Jump()
{
	Super::Jump();
}

void AOpenWorldCharacter::Roll(const FInputActionValue& Value)
{
	if(ActionState == EActionState::EAS_Rolling || ActionState == EActionState::EAS_Equipping ||
		CharacterMovementComponent->IsFalling()) return;

	PlayRollMontage(Value);
}

void AOpenWorldCharacter::PlayRollMontage(const FInputActionValue& Value)
{
	if(RollMontage)
	{
		const FVector MovementVector = Value.Get<FVector>();
		DirectionToRoll = MovementVector.GetSafeNormal(0.f);

		const FVector CurrentActorLocation = GetActorLocation();
		
		const FRotator DirectionToFace = UKismetMathLibrary::FindLookAtRotation(
			CurrentActorLocation,
			CurrentActorLocation + GetLastMovementInputVector()
		);
		
		SetActorRelativeRotation(DirectionToFace);
	
		ActionState = EActionState::EAS_Rolling;
		AnimInstance->Montage_Play(RollMontage);
	}
}

void AOpenWorldCharacter::SpeedUpRoll()
{
	CharacterMovementComponent->MaxWalkSpeed = RollSpeed;
}

void AOpenWorldCharacter::SlowDownRoll()
{
	CharacterMovementComponent->MaxWalkSpeed = GetMaxWalkSpeed();
}

bool AOpenWorldCharacter::CanAttack()
{
	return (ActionState == EActionState::EAS_Unoccupied || ActionState == EActionState::EAS_AttackEnd)
	&& EquipState != EEquipState::ECS_Unequipped;
}

void AOpenWorldCharacter::ResetActionState()
{
	ActionState = EActionState::EAS_Unoccupied;
	CharacterMovementComponent->MaxWalkSpeed = GetMaxRunSpeed();
}
