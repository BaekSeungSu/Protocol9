// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FogManager.generated.h"

UCLASS()
class PROTOCOL9_API AFogManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFogManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Fog Value")
	float FogDensity = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Fog Value")
	float SecondaryFogDensity = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Fog Value")
	float FogMaxOpacity = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Fog Value")
	bool bEnableVolumetricFog = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Fog Value")
	float ViewDistance=10000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Fog Value")
	float volumetricStartDistance = 3000.0f;

private:
	class UExponentialHeightFogComponent* FogComponent;
};
