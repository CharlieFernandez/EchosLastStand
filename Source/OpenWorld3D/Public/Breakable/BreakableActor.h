// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BreakableActor.generated.h"

class UGeometryCollectionComponent;

UCLASS()
class OPENWORLD3D_API ABreakableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ABreakableActor();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
    TObjectPtr<UGeometryCollectionComponent> GeometryCollectionComponent;
};
