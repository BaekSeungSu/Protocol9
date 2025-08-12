#include "Weapons/Weapon_Pulse.h"

AWeapon_Pulse::AWeapon_Pulse()
{
	bIsFullAuto = false;
}

void AWeapon_Pulse::PrimaryFire_Implementation()
{
	Super::PrimaryFire_Implementation();
}

void AWeapon_Pulse::Reload_Implementation()
{
	Super::Reload_Implementation();
}
