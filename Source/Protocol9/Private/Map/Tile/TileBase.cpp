// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/Tile/TileBase.h"
#include "Components/InstancedStaticMeshComponent.h"

// Sets default values
ATileBase::ATileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	SetRootComponent(SceneComponent);
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMeshComponent->SetupAttachment(RootComponent);
	
}

// Called when the game starts or when spawned
void ATileBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATileBase::ClearPreviousInstances()
{
	for (auto mesh : SpawnedInstance)
	{
		if (mesh)
		{
			mesh->ClearInstances();
			mesh->DestroyComponent();
		}
	}
	SpawnedInstance.Empty();
}

void ATileBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	ClearPreviousInstances();

	UWorld* World = GetWorld();
	if (!World||InstanceMeshList.IsEmpty()) return;

	for (UStaticMesh* Mesh : InstanceMeshList)
	{
		if (!Mesh) continue;

		UInstancedStaticMeshComponent* InstanceMeshComponent = NewObject<UInstancedStaticMeshComponent>(this);
		InstanceMeshComponent ->RegisterComponent();
		InstanceMeshComponent ->SetStaticMesh(Mesh);
		InstanceMeshComponent ->AttachToComponent(RootComponent,FAttachmentTransformRules::KeepRelativeTransform);
		AddInstanceComponent(InstanceMeshComponent);
		SpawnedInstance.Add(InstanceMeshComponent);

		int32 spawned = 0;
		int32 MaxAttempts =SpawnQuantity*10;

		while (spawned<SpawnQuantity && MaxAttempts-- >0)
		{
			/*
			FVector LocalPos = FVector(
				FMath::FRandRange(-SpawnAreaSize.X * 0.5f, SpawnAreaSize.X * 0.5f),
				FMath::FRandRange(-SpawnAreaSize.Y * 0.5f, SpawnAreaSize.Y * 0.5f),
				0.f
			);
			FVector WorldPos = ActorToWorld().TransformPosition(LocalPos);

			// Overlap 검사
			TArray<AActor*> Overlaps;
			UKismetSystemLibrary::SphereOverlapActors(
				World,
				WorldPos,
				SpawnRadius,
				{UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic)},
				nullptr,
				TArray<AActor*>(),
				Overlaps
			);

			bool bBlocked = false;
			for (AActor* Hit : Overlaps)
			{
				if (Hit && Hit->ActorHasTag("building"))
				{
					bBlocked = true;
					break;
				}
			}

			if (!bBlocked)
			{
				FTransform SpawnTransform;
				SpawnTransform.SetLocation(LocalPos);
				SpawnTransform.SetRotation(FQuat::MakeFromEuler(FVector(0, 0, FMath::FRandRange(0.f, 360.f))));
				ISMC->AddInstance(SpawnTransform);
				Spawned++;
			}
			 */
		}
	}
}





