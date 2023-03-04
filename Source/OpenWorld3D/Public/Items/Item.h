// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Items/ItemState.h"
#include "Item.generated.h"

class USphereComponent;
class USoundBase;

UCLASS()
class OPENWORLD3D_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();
	
	virtual void Tick(float DeltaTime) override;
	FORCEINLINE UStaticMeshComponent* GetStaticMeshComponent() const { return ItemMesh; }
	FORCEINLINE UMeshComponent* GetMesh() const { return ItemMesh; };
	FORCEINLINE USoundBase* GetPickUpSound() const { return PickUpSound; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//BlueprintCallable || BlueprintPure
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

	EItemState ItemState = EItemState::EIS_Hovering;

private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadonly, meta = (AllowPrivateAccess = "true"))
	float RunningTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters", meta = (AllowPrivateAccess = "true"))
	float Amplitude;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters", meta = (AllowPrivateAccess = "true"))
	float TimeConstant;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USoundBase> PickUpSound;

	UPROPERTY(EditAnywhere)
	float SpinSpeed = 100;
};
