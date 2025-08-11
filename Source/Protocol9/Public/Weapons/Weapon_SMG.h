#pragma once

#include "CoreMinimal.h"
#include "Weapons/WeaponBase.h"
#include "Weapon_SMG.generated.h"

UCLASS()
class PROTOCOL9_API AWeapon_SMG : public AWeaponBase
{
	GENERATED_BODY()
public:
	AWeapon_SMG();
	virtual void PrimaryFire_Implementation() override;
	virtual void StopFire_Implementation() override;
	virtual void Reload_Implementation() override;
	
protected:
	virtual void BeginPlay() override;

	FTimerHandle FireTimerHandle;
	
};
