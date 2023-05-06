#pragma once

UENUM(BlueprintType)
enum class EEquipState: uint8
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),
	ECS_Equipped UMETA(DisplayName = "Equipped")
};

UENUM(BlueprintType)
enum class EActionState: uint8
{
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	EAS_AttackEnd UMETA(DisplayName = "Attack End"),
	EAS_Equipping UMETA(DisplayName = "Equipping"),
	EAS_Dashing UMETA(DisplayName = "Dashing"),
	EAS_DashNearingEnd UMETA(DisplayName = "Dash Nearing End"),
	EAS_HitReact UMETA(DisplayName = "Hit React")
};

UENUM()
enum class EEnemyState: uint8
{
	EES_Dead UMETA(DisplayName = "Dead"),
	EES_Patrolling UMETA(DisplayName = "Patrolling"),
	EES_Chasing UMETA(DisplayName = "Chasing"),
	EES_ConsideringAttack UMETA(DisplayName = "Considering Attack"),
	EES_Engaged UMETA(DisplayName = "Engaged")
};