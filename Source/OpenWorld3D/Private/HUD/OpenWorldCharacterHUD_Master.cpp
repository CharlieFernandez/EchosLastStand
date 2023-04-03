// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/OpenWorldCharacterHUD_Master.h"
#include "HUD/OpenWorldCharacterHUD.h"

#include "Blueprint/UserWidget.h"

void AOpenWorldCharacterHUD_Master::BeginPlay()
{
	if(const UWorld* World = GetWorld())
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();

		if(PlayerController && OpenWorldCharacterHUDClass)
		{
			OpenWorldCharacterHUD = CreateWidget<UOpenWorldCharacterHUD>(PlayerController, OpenWorldCharacterHUDClass);
			OpenWorldCharacterHUD->AddToViewport();
		}
	}
}
