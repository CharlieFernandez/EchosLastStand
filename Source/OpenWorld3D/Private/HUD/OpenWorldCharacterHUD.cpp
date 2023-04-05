// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/OpenWorldCharacterHUD.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UOpenWorldCharacterHUD::SetHealthPercent(float Percent) const
{
	if(HealthProgressBar)
	{
		HealthProgressBar->SetPercent(Percent);
	}
}

void UOpenWorldCharacterHUD::SetStaminaPercent(float Percent) const
{
	if(StaminaProgressBar)
	{
		StaminaProgressBar->SetPercent(Percent);
	}
}

void UOpenWorldCharacterHUD::SetGold(int32 Gold) const
{
	if(GoldText)
	{
		const FString String = FString::Printf(TEXT("%d"), Gold);
		const FText Text = FText::FromString(String);
		GoldText->SetText(Text);
	}
}

void UOpenWorldCharacterHUD::SetSouls(int32 Souls) const
{
	if(SoulsText)
	{
		const FString String = FString::Printf(TEXT("%d"), Souls);
		const FText Text = FText::FromString(String);
		SoulsText->SetText(Text);
	}
}
