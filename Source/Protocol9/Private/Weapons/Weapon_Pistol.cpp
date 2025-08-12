#include "Weapons/Weapon_Pistol.h"

AWeapon_Pistol::AWeapon_Pistol()
{
	bIsFullAuto = false;
}

void AWeapon_Pistol::PrimaryFire_Implementation()
{
	Super::PrimaryFire_Implementation();
	
}

void AWeapon_Pistol::Reload_Implementation()
{
	Super::Reload_Implementation();
}

void AWeapon_Pistol::BeginPlay()
{
	Super::BeginPlay();
}
