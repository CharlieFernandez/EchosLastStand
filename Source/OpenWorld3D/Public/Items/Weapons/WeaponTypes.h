// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

UENUM(BlueprintType)
enum class EWeaponType: uint8
{
	EWT_Light UMETA(DisplayName = "Light"),
	EWT_Heavy UMETA(DisplayName = "Heavy")
};