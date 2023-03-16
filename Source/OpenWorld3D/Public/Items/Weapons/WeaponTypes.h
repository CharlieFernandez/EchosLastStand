// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

UENUM(BlueprintType)
enum class EWeaponType: uint8
{
	EWT_OneHanded UMETA(DisplayName = "One-Handed"),
	EWT_TwoHanded UMETA(DisplayName = "Two-Handed")
};