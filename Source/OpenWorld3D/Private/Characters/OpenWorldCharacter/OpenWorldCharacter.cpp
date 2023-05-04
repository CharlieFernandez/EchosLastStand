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
#include "NiagaraComponent.h"
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

   HealthRadiusSphereComponent = CreateDefaultSubobject<USphereComponent>("Combat Radius");
   HealthRadiusSphereComponent->SetupAttachment(GetRootComponent());
   
   GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
   GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
   GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
   GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);

   HairMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("Hair Mesh Component");
   HairMeshComponent->SetupAttachment(GetMesh(), FName(TEXT("head")));

   DashNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("Dash VFX");
   DashNiagaraComponent->SetupAttachment(GetRootComponent());
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

   if(Attributes && OpenWorldCharacterHUD && !IsDashing())
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

   FrameDashStaminaDeduction = DashStaminaCost * DeltaTime;

   if(IsDashing())
   {
      if(Attributes)
      {         
         if(Attributes->GetCurrentStamina() >= FrameDashStaminaDeduction && OpenWorldCharacterHUD)
         {
            Attributes->UpdateStamina(-FrameDashStaminaDeduction);
            OpenWorldCharacterHUD->SetStaminaPercent(Attributes->GetCurrentStaminaPercent());

            if(GetLastMovementInputVector() == FVector::Zero())
            {
               Move(LastNonZeroMovementInput.GetSafeNormal());
            }
         }
         if(Attributes->GetCurrentStamina() < FrameDashStaminaDeduction)
         {
            DashEnd();
         }        
      }     
   }
}

void AOpenWorldCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
   Super::SetupPlayerInputComponent(PlayerInputComponent);

   if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
   {
      EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &AOpenWorldCharacter::MoveInput);
      EnhancedInputComponent->BindAction(LookAroundAction, ETriggerEvent::Triggered, this, &AOpenWorldCharacter::LookAround);
      EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AOpenWorldCharacter::Jump);
      EnhancedInputComponent->BindAction(EKeyPressedAction, ETriggerEvent::Triggered, this, &AOpenWorldCharacter::ObtainOrEquip);
      EnhancedInputComponent->BindAction(AttackPressedAction, ETriggerEvent::Triggered, this, &AOpenWorldCharacter::Attack);
      EnhancedInputComponent->BindAction(DashPressedAction, ETriggerEvent::Triggered, this, &AOpenWorldCharacter::DashInput);
      EnhancedInputComponent->BindAction(DashReleasedAction, ETriggerEvent::Triggered, this, &AOpenWorldCharacter::DashEnd);
      EnhancedInputComponent->BindAction( LockOnAction, ETriggerEvent::Triggered, this, &AOpenWorldCharacter::LockOn);
      EnhancedInputComponent->BindAction( LockOffAction, ETriggerEvent::Triggered, this, &AOpenWorldCharacter::LockOff);
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

void AOpenWorldCharacter::Move(FVector2D MovementVector)
{
   const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);
   const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
   const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

   AddMovementInput(ForwardDirection, MovementVector.Y);
   AddMovementInput(RightDirection, MovementVector.X);
}

void AOpenWorldCharacter::MoveInput(const FInputActionValue& Value)
{  
   if( !IsAlive() || !IsUnoccupied() && !IsAttacking() && !IsDashing()) return;

   LastNonZeroMovementInput = Value.Get<FVector2D>();
   
   if(IsUnoccupied())
   {     
      Move(LastNonZeroMovementInput);
   }
   else if(IsDashing())
   {
      Move(LastNonZeroMovementInput.GetSafeNormal());
   }
   else if(IsAttacking())
   {
      const FRotator DirectionToFace = UKismetMathLibrary::FindLookAtRotation(
         FVector::ZeroVector,
         FVector(LastNonZeroMovementInput.Y, LastNonZeroMovementInput.X, 0)
      );

      const FRotator CameraRotation = CameraBoom->GetComponentRotation();
      const FRotator CameraYawRotation (0, CameraRotation.Yaw, 0);
      SetActorRotation(DirectionToFace + CameraYawRotation);
   }
}

void AOpenWorldCharacter::LookAround(const FInputActionValue& Value)
{  
   const FVector2D InputRotationValue = Value.Get<FVector2D>();
   const FVector2D RotationValue = InputRotationValue * CameraRotationSpeed;

   if(Controller && !InputRotationValue.IsZero())
   {
      if
      (
         (RotationValue.Y < 0 && CameraBoom->GetComponentLocation().Z > GetActorLocation().Z - LowestCameraPositionRelativeToPlayer) ||
         (RotationValue.Y > 0 && CameraBoom->GetComponentLocation().Z < GetActorLocation().Z + HighestCameraPositionRelativeToPlayer)
      )
      {
         AddControllerPitchInput(RotationValue.Y);
      }
      
      AddControllerYawInput(RotationValue.X);
   }
}

void AOpenWorldCharacter::ObtainOrEquip()
{
   if(!IsAlive()) return;
   
   if(    AWeapon* Weapon = Cast<AWeapon>(OverlappedItem))
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

void AOpenWorldCharacter::DashInput(const FInputActionValue& Value)
{
   if(!IsUnoccupied() || !IsAlive()) return;

   if(OpenWorldCharacterHUD && Attributes->GetCurrentStamina() >= FrameDashStaminaDeduction && HasMovementInput())
   {
      ActionState = EActionState::EAS_Dashing;

      CharacterMovementComponent->MaxWalkSpeed = DashSpeed;
      CharacterMovementComponent->MaxAcceleration = 50000;

      if(DashNiagaraComponent)
      {
         DashNiagaraComponent->Activate();
         ToggleAllMeshVisibility(false);
      }
   }
}

void AOpenWorldCharacter::DashEnd()
{
   if(DashNiagaraComponent) DashNiagaraComponent->Deactivate();

   ToggleAllMeshVisibility(true);
   CharacterMovementComponent->MaxWalkSpeed = GetMaxSprintSpeed();
   CharacterMovementComponent->MaxAcceleration = 2048;
   ResetActionState();
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

void AOpenWorldCharacter::ToggleAllMeshVisibility(bool IsVisible) const
{
   GetMesh()->SetVisibility(IsVisible);

   if(HairMeshComponent) HairMeshComponent->SetVisibility(IsVisible);
   if(GetWeaponHeld()) GetWeaponHeld()->GetMesh()->SetVisibility(IsVisible);
}