#include "Weapons/Weapon_Pistol.h"

AWeapon_Pistol::AWeapon_Pistol()
{
	
}

void AWeapon_Pistol::PrimaryFire_Implementation()
{
	Super::PrimaryFire_Implementation();
	UE_LOG(LogTemp, Warning, TEXT("Pistol Fired!!"));
	FireAction();
}

void AWeapon_Pistol::Reload_Implementation()
{
	Super::Reload_Implementation();
}

void AWeapon_Pistol::BeginPlay()
{
	Super::BeginPlay();
}
