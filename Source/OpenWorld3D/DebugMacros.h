#pragma once
#include "DrawDebugHelpers.h"

#define DRAW_SPHERE(ActorLocation) if(GetWorld()) DrawDebugSphere(GetWorld(), ActorLocation, 25.f, 12.f, FColor::Blue, true)
#define DRAW_SPHERE_SINGLE_FRAME(ActorLocation) if(GetWorld()) DrawDebugSphere(GetWorld(), ActorLocation, 25.f, 12.f, FColor::Blue, false, -1.f)
#define DRAW_LINE(StartLocation, EndLocation) if(GetWorld()) DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor:: Green, true, -1.f, 0u, 1.f)
#define DRAW_LINE_SINGLE_FRAME(StartLocation, EndLocation) if(GetWorld()) DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor:: Green, false, -1.f, 0u, 1.f)
#define DRAW_POINT(ActorLocation) if(GetWorld()) DrawDebugPoint(GetWorld(), ActorLocation, 15.f, FColor::Red, true)
#define DRAW_POINT_SINGLE_FRAME(ActorLocation) if(GetWorld()) DrawDebugPoint(GetWorld(), ActorLocation, 15.f, FColor::Red, false, -1.f)

#define DRAW_VECTOR(StartLocation, EndLocation) if(GetWorld())	\
{	\
	DRAW_LINE(StartLocation, EndLocation);	\
	DRAW_POINT(EndLocation);	\
}

#define DRAW_VECTOR_SINGE_FRAME(StartLocation, EndLocation) if(GetWorld())		\
{	\
	DRAW_LINE_SINGLE_FRAME(StartLocation, EndLocation);	\
	DRAW_POINT_SINGLE_FRAME(EndLocation);	\
}