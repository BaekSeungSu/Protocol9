#include "Weapons/Weapon_Pistol.h"
#include "Character/CharacterStateMachine.h"
#include "Character/MainCharacter.h"

AWeapon_Pistol::AWeapon_Pistol()
{
	bIsFullAuto = false;
}

void AWeapon_Pistol::PrimaryFire_Implementation()
{
	Super::PrimaryFire_Implementation();
	if (OwningCharacter && OwningCharacter->GetStateMachine())
	{
		OwningCharacter->GetStateMachine()->SetState(ECharacterState::Idle);
	}
}

void AWeapon_Pistol::Reload_Implementation()
{
	Super::Reload_Implementation();
}

void AWeapon_Pistol::BeginPlay()
{
	Super::BeginPlay();
}
