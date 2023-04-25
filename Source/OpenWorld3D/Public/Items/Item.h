// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/AudioComponent.h"
#include "GameFramework/Actor.h"
#include "Interfaces/PickUpInterface.h"
#include "Items/ItemState.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Item.generated.h"

class USphereComponent;
class USoundBase;
class UNiagaraComponent;
class UNiagaraSystem;
class UAudioComponent;

UCLASS()
class OPENWORLD3D_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();
	
	virtual void Tick(float DeltaTime) override;

	void PickUpItem();
	
	FORCEINLINE UStaticMeshComponent* GetStaticMeshComponent() const { return ItemMesh; }
	FORCEINLINE UMeshComponent* GetMesh() const { return ItemMesh; };
	FORCEINLINE USoundBase* GetPickUpSound() const { return PickUpSound; }
	FORCEINLINE EItemState GetItemState() const { return ItemState; }

protected:
	TObjectPtr<UNiagaraComponent> UncollectedParticlesInGame;
	
	virtual void BeginPlay() override;
	void SpawnPickUpEffect() const;
	void PlayUncollectedSound();
	void StopUncollectedSound() const;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> UncollectedParticles;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> CollectedParticles;
	
	UFUNCTION(BlueprintPure)
	float TransformSine() const { return Amplitude * FMath::Sin(RunningTime * TimeConstant); }

	UFUNCTION()
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereComponent;

	UPROPERTY(EditDefaultsOnly)
	EItemState ItemState;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> UncollectedSound;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> PickUpSound;

private:
	UAudioComponent* UncollectedSoundPlayed;
	void PlayPickUpSound() const;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadonly, meta = (AllowPrivateAccess = "true"))
	float RunningTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hover", meta = (AllowPrivateAccess = "true"))
	float Amplitude;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hover", meta = (AllowPrivateAccess = "true"))
	float TimeConstant;
	UPROPERTY(EditAnywhere, Category = "Hover")
	float SpinSpeed = 100;

	// DEBUGS
	UPROPERTY(EditDefaultsOnly, Category = Debug, meta = (AllowPrivateAccess = "true"))
	bool DrawItemRingLineTrace;
};
