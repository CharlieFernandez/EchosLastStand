// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Souls.generated.h"

class UNiagaraSystem;

/**
 * 
 */
UCLASS()
class OPENWORLD3D_API ASouls : public AItem
{
	GENERATED_BODY()

public:
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	FORCEINLINE uint32 GetSoulCount() const { return SoulCount; }
	FORCEINLINE void SetSoulCount(const int SoulValue) { SoulCount = SoulValue; }

private:
	UPROPERTY(EditAnywhere)
	int32 SoulCount;
};
