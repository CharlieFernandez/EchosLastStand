// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OpenWorldCharacterHUD.generated.h"

class UTextBlock;
class UProgressBar;

UCLASS()
class OPENWORLD3D_API UOpenWorldCharacterHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:	
	void SetHealthPercent(float Percent) const;
	void SetEnergyPercent(float Percent) const;
	void SetGold(int32 Gold) const;
	void SetSouls(int32 Souls) const;
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> EnergyProgressBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> GoldText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SoulsText;
};
