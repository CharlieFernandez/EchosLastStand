// Fill out your copyright notice in the Description page of Project Settings.


#include "MyUtilities.h"

#include "Kismet/KismetMathLibrary.h"

bool MyUtilities::InTargetRange(const float Radius, const AActor* SourceActor, const AActor* TargetActor)
{
	const double DistanceFromTarget = (SourceActor->GetActorLocation() - TargetActor->GetActorLocation()).Length();
	return DistanceFromTarget <= Radius;
}