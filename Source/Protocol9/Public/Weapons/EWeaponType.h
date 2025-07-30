#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Pistol UMETA(DisplayName = "Pistol"),
	SMG UMETA(DisplayName = "SMG"),
	Rifle UMETA(DisplayName = "Rifle"),
	RocketLauncher UMETA(DisplayName = "Rocket Launcher")
};