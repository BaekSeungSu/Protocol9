#include "Weapons/Weapon_SMG.h"
#include "Weapons/Data/WeaponData.h"

AWeapon_SMG::AWeapon_SMG()
{
	bIsFullAuto = true;
}

void AWeapon_SMG::PrimaryFire_Implementation()
{
	if (!CurrentWeaponData) return;
		
	
	const float TimeBetweenShots = 60.0f / CurrentWeaponData->FireRate;
	GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &AWeaponBase::FireAction, TimeBetweenShots, true, 0.0f);

}

void AWeapon_SMG::StopFire_Implementation()
{
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
}

void AWeapon_SMG::Reload_Implementation()
{
	Super::Reload_Implementation();
}

void AWeapon_SMG::BeginPlay()
{
	Super::BeginPlay();
}
