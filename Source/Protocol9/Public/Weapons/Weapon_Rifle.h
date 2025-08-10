#pragma once

#include "CoreMinimal.h"
#include "Weapons/WeaponBase.h"
#include "Weapon_Rifle.generated.h"


UCLASS()
class PROTOCOL9_API AWeapon_Rifle : public AWeaponBase
{
	GENERATED_BODY()
public:
	AWeapon_Rifle();
	virtual void PrimaryFire_Implementation() override;
	virtual void StopFire_Implementation() override;
	virtual void Reload_Implementation() override;
	
protected:
	virtual void BeginPlay() override;

	FTimerHandle FireTimerHandle;
};
