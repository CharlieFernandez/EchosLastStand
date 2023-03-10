// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

static class OPENWORLD3D_API MyUtilities
{
public:
	static bool InTargetRange(const float Radius, const AActor* SourceActor, const AActor* TargetActor);
};
