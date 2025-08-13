// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Pickup_Weapon.h"

// Sets default values
APickup_Weapon::APickup_Weapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APickup_Weapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickup_Weapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

