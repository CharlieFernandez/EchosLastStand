// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "OpenWorldCharacterHUD_Master.generated.h"

class UOpenWorldCharacterHUD;

UCLASS()
class OPENWORLD3D_API AOpenWorldCharacterHUD_Master : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

private:
	TObjectPtr<UOpenWorldCharacterHUD> OpenWorldCharacterHUD;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UOpenWorldCharacterHUD> OpenWorldCharacterHUDClass;
};
