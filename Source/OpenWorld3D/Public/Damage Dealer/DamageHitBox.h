// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedStruct.h"
#include "DamageHitBox.generated.h"

class UPrimitiveComponent;

USTRUCT(BlueprintType)
struct FDamageHitBox
{
	GENERATED_BODY()

public:
	FDamageHitBox();

	FORCEINLINE UPrimitiveComponent* GetHitBox() const { return HitBox; }
	FORCEINLINE float GetDamage() const { return Damage; }
	FORCEINLINE USoundBase* GetHitSFX() const { return HitSFX; }
	FORCEINLINE float GetImpactPauseTime() const { return ImpactPauseTime; }

private:	
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPrimitiveComponent> HitBox;
	
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Damage;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USoundBase> HitSFX;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float ImpactPauseTime;
};