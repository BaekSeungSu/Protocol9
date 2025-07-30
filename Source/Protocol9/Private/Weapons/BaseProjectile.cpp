#include "Weapons/BaseProjectile.h"

ABaseProjectile::ABaseProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ABaseProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

