#pragma once

UENUM(BlueprintType)
enum class EProjectileDamageType : uint8
{
	Point UMETA(DisplayName = "Point"),
	Radial UMETA(DisplayName = "Radial")
};
