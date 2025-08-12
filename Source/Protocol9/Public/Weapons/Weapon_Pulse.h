#pragma once

#include "CoreMinimal.h"
#include "Weapons/WeaponBase.h"
#include "Weapon_Pulse.generated.h"


UCLASS()
class PROTOCOL9_API AWeapon_Pulse : public AWeaponBase
{
	GENERATED_BODY()
public:
	AWeapon_Pulse();
	virtual void PrimaryFire_Implementation() override;
	virtual void Reload_Implementation() override;
};
