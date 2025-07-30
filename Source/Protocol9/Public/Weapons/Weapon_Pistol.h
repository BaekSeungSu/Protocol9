#pragma once

#include "CoreMinimal.h"
#include "Weapons/WeaponBase.h"
#include "Weapon_Pistol.generated.h"

UCLASS()
class PROTOCOL9_API AWeapon_Pistol : public AWeaponBase
{
	GENERATED_BODY()

public:
	AWeapon_Pistol();
	virtual void PrimaryFire_Implementation() override;
	virtual void Reload_Implementation() override;

protected:
	virtual void BeginPlay() override;
};
