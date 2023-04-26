// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/OpenWorldCharacter/OpenWorldCharacter.h"

#include "Enemy.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputState.h"
#include "MyUtilities.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/AttributeComponent.h"
#include "Components/LockOnComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/OpenWorldCharacterHUD.h"
#include "HUD/OpenWorldCharacterHUD_Master.h"
#include "Items/Souls.h"
#include "Items/Treasures/Treasure.h"
#include "Items/Weapons/EquipActionState.h"
#include "Items/Weapons/Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystem.h"

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

	LockOnComponent = CreateDefaultSubobject<ULockOnComponent>(TEXT("Lock-On Component"));
	LockOnComponent->InitializeVariables();

	HealthRadiusSphereComponent = CreateDefaultSubobject<USphereComponent>("Combat Radius");
	HealthRadiusSphereComponent->SetupAttachment(GetRootComponent());
	
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
}

void AOpenWorldCharacter::BindHealthRadiusSphereComponents()
{
	HealthRadiusSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AOpenWorldCharacter::OnSphereBeginOverlap);
	HealthRadiusSphereComponent->OnComponentEndOverlap.AddDynamic(this, &AOpenWorldCharacter::OnSphereEndOverlap);
}

void AOpenWorldCharacter::InitializeHUD(const APlayerController* PlayerController)
{
	if(const AOpenWorldCharacterHUD_Master* OpenWorldCharacterHUD_Master = Cast<AOpenWorldCharacterHUD_Master>(PlayerController->GetHUD()))
	{
		OpenWorldCharacterHUD = OpenWorldCharacterHUD_Master->GetOpenWorldCharacterHUD();

		if(OpenWorldCharacterHUD)
		{
			OpenWorldCharacterHUD->SetHealthPercent(Attributes->GetCurrentHealthPercent());
			OpenWorldCharacterHUD->SetStaminaPercent(Attributes->GetCurrentStaminaPercent());
			OpenWorldCharacterHUD->SetStaminaPercent(1.f);
			OpenWorldCharacterHUD->SetGold(0);
			OpenWorldCharacterHUD->SetSouls(0);
		}
	}
}

void AOpenWorldCharacter::BeginPlay()
{
	SpringArm->TargetArmLength = DefaultCameraDistance;
	LockOnComponent->InitializeVariablesOnBeginPlay(DefaultCameraDistance);
	Super::BeginPlay();
	Tags.AddUnique(PlayerTag);
	BindHealthRadiusSphereComponents();
	
	if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = PlayerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->AddMappingContext(OpenWorldCharacterContext, 0);
		}

		InitializeHUD(PlayerController);
	}
}

void AOpenWorldCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if(ActionState == EActionState::EAS_Rolling)
	{
		const FVector RollDisplacement = DirectionToRoll * RollSpeed * DeltaTime;

		AddMovementInput(GetActorForwardVector(), RollDisplacement.Length());
	}

	if(Attributes && OpenWorldCharacterHUD)
	{
		Attributes->RegenerateStamina(DeltaTime);
		OpenWorldCharacterHUD->SetStaminaPercent(Attributes->GetCurrentStaminaPercent());
	}

	const AEnemy* Target = Cast<AEnemy>(LockOnComponent->LockedOnTarget);
	
	if(Target && !Target->IsAlive())
	{		
		LockOnComponent->Unlock();
	}

	if(LockOnComponent->LockedOnTarget)
	{
		CombatTarget = LockOnComponent->LockedOnTarget;
	}
	else
	{
		CombatTarget = nullptr;
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
		EnhancedInputComponent -> BindAction(RollAction, ETriggerEvent::Triggered, this, &AOpenWorldCharacter::Roll);
		EnhancedInputComponent -> BindAction( LockOnAction, ETriggerEvent::Triggered, this, &AOpenWorldCharacter::LockOn);
		EnhancedInputComponent -> BindAction( LockOffAction, ETriggerEvent::Triggered, this, &AOpenWorldCharacter::LockOff);
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
	if( !IsAlive() || !IsUnoccupied() && !IsAttacking()) return;
	
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if(MovementVector.IsZero()) return;

	if(ActionState == EActionState::EAS_Unoccupied)
	{
		const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);	
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);	
	}
	else
	{
		const FRotator DirectionToFace = UKismetMathLibrary::FindLookAtRotation(
			FVector::ZeroVector,
			FVector(MovementVector.Y, MovementVector.X, 0)
		);

		const FRotator CameraRotation = CameraBoom->GetComponentRotation();
		const FRotator CameraYawRotation (0, CameraRotation.Yaw, 0);
		SetActorRotation(DirectionToFace + CameraYawRotation);
	}
}

void AOpenWorldCharacter::LookAround(const FInputActionValue& Value)
{	
	const FVector2D InputRotationValue = Value.Get<FVector2D>();
	const FVector2d RotationValue = InputRotationValue * CameraRotationSpeed;

	if(Controller && !InputRotationValue.IsZero())
	{		
		AddControllerPitchInput(RotationValue.Y);
		AddControllerYawInput(RotationValue.X);
	}
}

void AOpenWorldCharacter::ObtainOrEquip()
{
	if(!IsAlive()) return;
	
	if(	AWeapon* Weapon = Cast<AWeapon>(OverlappedItem))
	{
		PickUpWeapon(Weapon, Weapon->GetStaticMeshComponent(), rightHandItemSocket);
	}
	else
	{
		EquipOrUnequipWeapon();
	}
}

void AOpenWorldCharacter::EquipOrUnequipWeapon()
{
	if(GetWeaponHeld() && ActionState == EActionState::EAS_Unoccupied && IsAlive())
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

void AOpenWorldCharacter::PickUpWeapon(AWeapon* Weapon, UMeshComponent* WeaponMesh, FName SN)
{
	SetEquippedWeapon(Weapon, WeaponMesh, SN);
	Weapon->SetOwner(this);
	Weapon->SetInstigator(this);
	Weapon->ToggleWeaponState();
	Weapon->PickUpItem();
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
	if(!IsAlive() || !IsUnoccupied() ) return;
	
	Super::Jump();
}

void AOpenWorldCharacter::Roll(const FInputActionValue& Value)
{
	if(ActionState != EActionState::EAS_Unoccupied || CharacterMovementComponent->IsFalling() || !IsAlive()) return;

	if(Attributes && OpenWorldCharacterHUD && Attributes->GetCurrentStamina() > DodgeRollStaminaCost && HasMovementInput())
	{
		PlayRollMontage(Value);
		Attributes->UpdateStamina(-DodgeRollStaminaCost);
		OpenWorldCharacterHUD->SetStaminaPercent(Attributes->GetCurrentStaminaPercent());
	}
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
	CharacterMovementComponent->MaxWalkSpeed = GetMaxSprintSpeed();
}

void AOpenWorldCharacter::LockOn()
{
	LockOnComponent->Lock();
}

void AOpenWorldCharacter::LockOff()
{
	LockOnComponent->Unlock();
}

void AOpenWorldCharacter::GetHit_Implementation(const FVector ImpactPoint, const FVector InstigatorPosition)
{
	if(OpenWorldCharacterHUD && Attributes)
	{
		OpenWorldCharacterHUD->SetHealthPercent(Attributes->GetCurrentHealthPercent());
	}
	
	if(!IsAlive()) return;
	
	Super::GetHit_Implementation(ImpactPoint, InstigatorPosition);
}

void AOpenWorldCharacter::AddSouls(ASouls* Souls)
{
	if(Souls && Attributes)
	{
		const int32 SoulCount = Souls->GetSoulCount();
		Attributes->AddSouls(SoulCount);
		OpenWorldCharacterHUD->SetSouls(Attributes->GetTotalSouls());
	}
}

void AOpenWorldCharacter::AddGold(ATreasure* Treasure)
{
	if(Treasure && Attributes)
	{
		const int32 GoldCount = Treasure->GetTotalGold();
		Attributes->AddGold(GoldCount);
		OpenWorldCharacterHUD->SetGold(Attributes->GetTotalGold());
	}
}

bool AOpenWorldCharacter::HasMovementInput() const
{
	return FVector2d(GetLastMovementInputVector()) != FVector2d::Zero();
}

void AOpenWorldCharacter::SpawnHammerDownParticles()
{
	AActor* ActorOwner = GetOwner<AActor>();
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.AddUnique(GetWeaponHeld());
	ActorsToIgnore.AddUnique(ActorOwner);

	TArray<FHitResult> HitResults;
	FVector HammerPointStart = GetWeaponHeld()->GetMesh()->GetSocketLocation(FName("MiddleTop"));
	FVector HammerPointEnd = HammerPointStart;
	HammerPointStart.Z += 100;
	HammerPointEnd.Z -= 200;

	const EDrawDebugTrace::Type DebugTraceType = ShouldDrawHammerDownDebugTrace ? EDrawDebugTrace::Persistent : EDrawDebugTrace::None;
	
	FHitResult FloorHit = MyUtilities::GetLineTraceGroundImpactPoint(this, HammerPointStart, HammerPointEnd, DebugTraceType);

	if(FloorHit.GetActor() != nullptr)
	{
		FRotator RotationToPlantOnFloor = MyUtilities::GetRotationFromNormalVector(FloorHit);		
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, HammerDownParticles, FloorHit.ImpactPoint, RotationToPlantOnFloor);
	}
}