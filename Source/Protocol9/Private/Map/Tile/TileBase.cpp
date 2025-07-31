#include "Map/Tile/TileBase.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SceneComponent.h"

// Sets default values
ATileBase::ATileBase()
{
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

	GenerateInstances();
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

void ATileBase::GenerateInstances()
{
	ClearPreviousInstances();

	UWorld* World = GetWorld();
	if (!World || InstanceMeshList.IsEmpty())
		return;

	FBox TileBounds = StaticMeshComponent->Bounds.GetBox(); 
	FVector Center = TileBounds.GetCenter();
	FVector Extents = TileBounds.GetExtent();

	UE_LOG(LogTemp, Warning, TEXT("Tile Bounds Center.Z = %f"), Center.Z);

	for (UStaticMesh* Mesh : InstanceMeshList)
	{
		if (!Mesh) continue;

		UInstancedStaticMeshComponent* InstanceMeshComponent = NewObject<UInstancedStaticMeshComponent>(
			this,
			UInstancedStaticMeshComponent::StaticClass(),
			NAME_None,
			RF_Transient
		);

		if (!IsValid(InstanceMeshComponent)) continue;

		InstanceMeshComponent->SetupAttachment(RootComponent);
		InstanceMeshComponent->SetMobility(EComponentMobility::Movable);
		InstanceMeshComponent->SetStaticMesh(Mesh);
		InstanceMeshComponent->RegisterComponent();
		AddInstanceComponent(InstanceMeshComponent);
		SpawnedInstance.Add(InstanceMeshComponent);

		int32 Spawned = 0;
		int32 MaxAttempts = SpawnQuantity * 20;

		while (Spawned < SpawnQuantity && MaxAttempts-- > 0)
		{
			FVector SpawnLocation = FVector(
				FMath::RandRange(Center.X - Extents.X, Center.X + Extents.X),
				FMath::RandRange(Center.Y - Extents.Y, Center.Y + Extents.Y),
				Center.Z
			);

			TArray<AActor*> Overlaps;
			UKismetSystemLibrary::SphereOverlapActors(
				World,
				SpawnLocation,
				SpawnRadius,
				{ UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic) },
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
				FVector LocalLocation = ActorToWorld().InverseTransformPosition(SpawnLocation);
				FTransform InstanceTransform;
				InstanceTransform.SetLocation(LocalLocation);
				InstanceTransform.SetRotation(FQuat::MakeFromEuler(FVector(0.f, 0.f, FMath::FRandRange(0.f, 360.f))));
				InstanceMeshComponent->AddInstance(InstanceTransform);
				Spawned++;
			}
		}
	}
}
