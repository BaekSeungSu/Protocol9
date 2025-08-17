#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Default UMETA(DisplayName = "Default"),
	Pistol UMETA(DisplayName = "Pistol"),
	SMG UMETA(DisplayName = "SMG"),
	Rifle UMETA(DisplayName = "Rifle"),
	Pulse UMETA(DisplayName = "Pulse")
};