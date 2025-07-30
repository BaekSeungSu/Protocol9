#pragma once

UENUM(BlueprintType)
enum class EFireType : uint8
{
	HitScan UMETA(DisplayName = "Hit Scan"),
	Projectile UMETA(DisplayName = "Projectile")
};
