// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileBase.generated.h"

UCLASS()
class PROTOCOL9_API ATileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATileBase();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StaticMesh")
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StaticMesh | SceneComponent")
	USceneComponent* SceneComponent;

	UPROPERTY(EditAnywhere,Category="MeshList")
	TArray<UStaticMesh*> InstanceMeshList;

	UPROPERTY(EditAnywhere, Category ="Mesh Spawn Quantity")
	int32 SpawnQuantity = 10;

	UPROPERTY(EditAnywhere, Category ="Mesh Interval With Building")
	float SpawnRadius = 50.0f;	

	UPROPERTY(EditAnywhere, Category ="Mesh Spawn Area")
	FVector2D SpawnArae= FVector2D(0.0f, 0.0f);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
private:
	UPROPERTY(Transient)
	TArray<UInstancedStaticMeshComponent*> SpawnedInstance;

	void ClearPreviousInstances();
};
