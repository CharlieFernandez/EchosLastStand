// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PickUpInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPickUpInterface : public UInterface
{
	GENERATED_BODY()
};

class AItem;
class ASouls;
class ATreasure;

/**
 * 
 */
class OPENWORLD3D_API IPickUpInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void SetOverlappingItem(AItem* Item);
	virtual void AddSouls(ASouls* Souls);
	virtual void AddGold(ATreasure* Treasure);
};
