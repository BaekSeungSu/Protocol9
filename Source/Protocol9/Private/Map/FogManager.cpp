// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/FogManager.h"
#include "EngineUtils.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Engine/ExponentialHeightFog.h"

// Sets default values
AFogManager::AFogManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AFogManager::BeginPlay()
{
	Super::BeginPlay();

	//It가 end()일 때 그만
	for (TActorIterator<AExponentialHeightFog> It(GetWorld()); It; ++It)
	{
		FogComponent= It->GetComponent();
		break;
	}

	if (FogComponent)
	{
		FogComponent ->SetFogDensity(FogDensity);
		FogComponent ->SetSecondFogDensity(SecondaryFogDensity);
		FogComponent->SetFogMaxOpacity(FogMaxOpacity);
		FogComponent->SetVolumetricFog(bEnableVolumetricFog);
		FogComponent ->SetVolumetricFogDistance(ViewDistance);
		FogComponent->SetVolumetricFogStartDistance(volumetricStartDistance);
	}
}

// Called every frame
void AFogManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

