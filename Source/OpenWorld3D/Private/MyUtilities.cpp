// Fill out your copyright notice in the Description page of Project Settings.


#include "MyUtilities.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

bool MyUtilities::InTargetRange(const float Radius, const AActor* SourceActor, const AActor* TargetActor)
{
	if(SourceActor && TargetActor)
	{
		const double DistanceFromTarget = (SourceActor->GetActorLocation() - TargetActor->GetActorLocation()).Length();
		return DistanceFromTarget <= Radius;
	}
	
	return false;
}

FHitResult MyUtilities::GetLineTraceGroundImpactPoint(const UObject* WorldContext, FVector Start, FVector End, EDrawDebugTrace::Type TraceType)
{
	TArray<FHitResult> HitResults;
	UKismetSystemLibrary::LineTraceMulti(WorldContext, Start, End, ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(), TraceType, HitResults, true);

	FHitResult FloorHit;
	
	for(FHitResult HitResult: HitResults)
	{
		if(HitResult.GetActor()->ActorHasTag("Ground"))
		{
			FloorHit = HitResult;
			break;
		}
	}

	FloorHit.ImpactPoint.Z += 1.5f;

	return FloorHit;
}