#include "Weapons/Weapon_Rifle.h"
#include "Weapons/Data/WeaponData.h"

AWeapon_Rifle::AWeapon_Rifle()
{
	bIsFullAuto = true;
}

void AWeapon_Rifle::PrimaryFire_Implementation()
{
	if (!CurrentWeaponData) return;
		
	
	const float TimeBetweenShots = 60.0f / CurrentWeaponData->FireRate;
	GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &AWeaponBase::FireAction, TimeBetweenShots, true, 0.0f);

}

void AWeapon_Rifle::StopFire_Implementation()
{
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
}

void AWeapon_Rifle::Reload_Implementation()
{
	Super::Reload_Implementation();
}

void AWeapon_Rifle::BeginPlay()
{
	Super::BeginPlay();
}