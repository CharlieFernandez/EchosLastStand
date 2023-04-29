// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetSystemLibrary.h"

class OPENWORLD3D_API MyUtilities
{
public:
	static bool InTargetRange(const float Radius, const AActor* SourceActor, const AActor* TargetActor);
	static FHitResult GetLineTraceGroundImpactPoint(const UObject* WorldContext, FVector Start, FVector End, EDrawDebugTrace::Type DrawDebugType);
	static FRotator GetRotationFromNormalVector(FHitResult HitResult);
};
