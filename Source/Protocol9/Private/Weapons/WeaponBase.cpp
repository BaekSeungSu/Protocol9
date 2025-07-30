#include "Weapons/WeaponBase.h"
#include "Components/StaticMeshComponent.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMeshComp->SetSimulatePhysics(false);
	SetRootComponent(StaticMeshComp);
	
}

void AWeaponBase::PrimaryFire_Implementation()
{
	IWeaponInterface::PrimaryFire_Implementation();
}

void AWeaponBase::StopFire_Implementation()
{
	IWeaponInterface::StopFire_Implementation();
}

void AWeaponBase::Reload_Implementation()
{
	IWeaponInterface::Reload_Implementation();
}



void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

