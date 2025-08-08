// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileManager.generated.h"

UENUM()
enum class ETileShiftDirection : uint8
{
	None,
	Forward,   // +X
	Backward,  // -X
	Left,      // -Y
	Right      // +Y
};

class ATileBase;

UCLASS()
class PROTOCOL9_API ATileManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATileManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category="Character Class")
	ACharacter* Character;
	
	UPROPERTY(EditAnywhere, Category="Tile Size")
	FVector TileSize = FVector(10000.0f,10000.0f,30.0f);
	
	UPROPERTY(EditAnywhere, Category="NavMesh")
	class ANavMeshBoundsVolume* NavMeshBoundsVolume;
	
	TArray<ATileBase*> TileArray;

	int32 CurrentPlayerTileCoord;

	int32 GetPlayerTileCoord() const;

	void UpdateTilesIfNeeded();
	void ShiftTileArray(ETileShiftDirection Direction);
	void ShiftTileArray(const FIntPoint& Direction);
	void RearrangeTileArray(int32 NewCenterIndex);
};
