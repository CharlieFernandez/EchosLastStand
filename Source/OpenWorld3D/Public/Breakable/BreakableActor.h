// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"
#include "BreakableActor.generated.h"

class UCapsuleComponent;
class UGeometryCollectionComponent;
class ATreasure;

UCLASS()
class OPENWORLD3D_API ABreakableActor : public AActor, public IHitInterface
{
	GENERATED_BODY()
	
public:	
	ABreakableActor();
	virtual void Tick(float DeltaTime) override;
	virtual void GetHit_Implementation(const FVector ImpactPoint, const FVector InstigatorPosition) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
    TObjectPtr<UGeometryCollectionComponent> GeometryCollectionComponent;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<ATreasure>> Treasures;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> CapsuleComponent;
};