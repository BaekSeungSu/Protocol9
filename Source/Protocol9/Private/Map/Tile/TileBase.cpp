#include "Map/Tile/TileBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ATileBase::ATileBase()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	SetRootComponent(SceneComponent);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMeshComponent->SetupAttachment(RootComponent);
}

void ATileBase::BeginPlay()
{
	Super::BeginPlay();
	GenerateInstances();
}

void ATileBase::ClearPreviousInstances()
{
	for (auto Instance : SpawnedClass)
	{
		if (Instance)
		{
			Instance->Destroy();
		}
	}
	SpawnedClass.Empty();
}

void ATileBase::GenerateInstances()
{
	ClearPreviousInstances();

	UWorld* World = GetWorld();
	if (!World || SpawnSpecs.IsEmpty() || !IsValid(StaticMeshComponent))
		return;

	const FBox TileBounds = StaticMeshComponent->Bounds.GetBox();
	const FVector Center  = TileBounds.GetCenter();
	const FVector Extents = TileBounds.GetExtent();

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjTypes = {
		UEngineTypes::ConvertToObjectType(ECC_WorldStatic),
		UEngineTypes::ConvertToObjectType(ECC_WorldDynamic)
	};

	for (const FSpawnSpec& Spec : SpawnSpecs)
	{
		if (!Spec.Class) continue;

		int32 Spawned = 0;
		int32 MaxAttempts = Spec.Count * MaxAttemptsPerSpawn;

		while (Spawned < Spec.Count && MaxAttempts-- > 0)
		{
			FVector SpawnLocation(
				FMath::FRandRange(Center.X - Extents.X, Center.X + Extents.X),
				FMath::FRandRange(Center.Y - Extents.Y, Center.Y + Extents.Y),
				0
			);

			// === 컴포넌트 단위 겹침 검사 ===
			TArray<AActor*> IgnoreActors;
			IgnoreActors.Add(this);

			TArray<UPrimitiveComponent*> OverlapComps;
			UKismetSystemLibrary::SphereOverlapComponents(
				World,
				SpawnLocation,
				SpawnRadius,
				ObjTypes,
				nullptr,
				IgnoreActors,
				OverlapComps
			);

			bool bBlocked = false;
			for (UPrimitiveComponent* Comp : OverlapComps)
			{
				if (!IsValid(Comp)) continue;

				if (Comp->ComponentHasTag(FName("building")))
				{
					bBlocked = true;
					break;
				}

				AActor* OwnerActor = Comp->GetOwner();
				if (OwnerActor && OwnerActor->ActorHasTag(FName("building")))
				{
					bBlocked = true;
					break;
				}
			}

			if (bBlocked)
				continue;

			// === 스폰 ===
			FActorSpawnParameters Params;
			Params.Owner = this;
			Params.SpawnCollisionHandlingOverride =
				ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			AActor* NewActor = World->SpawnActor<AActor>(
				Spec.Class,
				SpawnLocation,
				FRotator(0, FMath::RandRange(0, 360), 0),
				Params
			);

			if (IsValid(NewActor))
			{
				auto PlaceOnSurface = [&](AActor* Actor) -> bool
				{
					if (!IsValid(Actor)) return false;

					const FVector Base = Actor->GetActorLocation();
					const FVector Start = Base + FVector(0, 0, 10000);
					const FVector End   = Base - FVector(0, 0, 10000);

					FCollisionQueryParams QP(SCENE_QUERY_STAT(SpawnTrace), false);
					QP.AddIgnoredActor(Actor);
					for (AActor* Ign : SpawnedClass)
						if (IsValid(Ign))
							QP.AddIgnoredActor(Ign);

					FCollisionObjectQueryParams OP;
					OP.AddObjectTypesToQuery(ECC_WorldStatic);
					OP.AddObjectTypesToQuery(ECC_WorldDynamic);

					FHitResult Hit;
					if (!GetWorld()->LineTraceSingleByObjectType(Hit, Start, End, OP, QP))
						return false;

					FBox Box;
					TArray<UActorComponent*> Tagged = Actor->GetComponentsByTag(
						UStaticMeshComponent::StaticClass(),
						FName("MainStaticMesh")
					);

					if (Tagged.Num() > 0)
					{
						if (UStaticMeshComponent* SMC = Cast<UStaticMeshComponent>(Tagged[0]))
						{
							Box = SMC->Bounds.GetBox();
						}
					}
					else
					{
						FVector Origin, Extent;
						Actor->GetActorBounds(true, Origin, Extent);
						Box = FBox(Origin - Extent, Origin + Extent);
					}

					FVector Loc = Actor->GetActorLocation();
					const float DeltaZ = Hit.ImpactPoint.Z - Box.Min.Z;
					Loc.Z += DeltaZ;
					Actor->SetActorLocation(Loc, true);

					return true;
				};

				PlaceOnSurface(NewActor);

				NewActor->AttachToComponent(StaticMeshComponent, FAttachmentTransformRules::KeepWorldTransform);
				SpawnedClass.Add(NewActor);
				Spawned++;
			}
		}
	}
}
void ATileBase::RebuildInstances()
{
	GenerateInstances();
}

#if WITH_EDITOR
void ATileBase::PrintIndex()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("Tile Index: %d"), TileIndex));
}
#endif
