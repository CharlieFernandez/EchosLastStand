#pragma once

UENUM()
enum class EEquipActionState: uint8
{
	EEAS_Unequip UMETA(DisplayName = "Unequip"),
	EEAS_Equip UMETA(DisplayName = "Equip")
};
