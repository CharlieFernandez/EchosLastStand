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

FHitResult MyUtilities::GetLineTraceGroundImpactPoint(const UObject* WorldContext, FVector Start, FVector End, EDrawDebugTrace::Type DrawDebugType)
{
	TArray<FHitResult> HitResults;
	const ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_WorldStatic);
	
	UKismetSystemLibrary::LineTraceMulti(WorldContext, Start, End, TraceType, false, TArray<AActor*>(), DrawDebugType, HitResults, true);

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

FRotator MyUtilities::GetRotationFromNormalVector(FHitResult HitResult)
{
	const FVector UpVector = FVector::UpVector;
	const FVector Axis = UpVector.Cross(HitResult.Normal).GetSafeNormal();
	const float Angle = FMath::Acos(UpVector.Dot(HitResult.Normal));
	return FQuat(Axis, Angle).Rotator();
}
