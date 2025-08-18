#include "Map/Tile/TileBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Weapons/Pickup_Weapon.h"

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
	SpawnWeapons();
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

void ATileBase::SpawnWeapons()
{
	UWorld* World = GetWorld();
	if (!World || WeaponSpecs.IsEmpty())
		return;

	const FBox TileBounds = StaticMeshComponent->Bounds.GetBox();
	const FVector TileCenter = TileBounds.GetCenter();
	const FVector TileExtent = TileBounds.GetExtent();

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypeQuery=
		{
			UEngineTypes::ConvertToObjectType(ECC_WorldStatic),
			UEngineTypes::ConvertToObjectType(ECC_WorldDynamic),
		};

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);

	for (const FWeaponSpec& Spec : WeaponSpecs)
	{
		if (!Spec.Class)
			continue;

		int32 Spawned = 0;

		while (Spawned < Spec.Count)
		{
			FVector SpawnLocation =
				{
					FMath::RandRange(TileCenter.X-TileExtent.X, TileCenter.X+TileExtent.X),
					FMath::RandRange(TileCenter.Y-TileExtent.Y, TileCenter.Y+TileExtent.Y)
					,Spec.ZOffset
				};
			
			TArray<AActor*> OverlappedActors;

			UKismetSystemLibrary::SphereOverlapActors(
				World,
				SpawnLocation,
				Spec.ClearRadius,
				ObjectTypeQuery,
				nullptr,
				IgnoreActors,
				OverlappedActors
				);

			if (OverlappedActors.Num() > 0)
				continue;

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.SpawnCollisionHandlingOverride =
				ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			AActor* NewWeaponActor= World->SpawnActor<AActor>(
				Spec.Class,
				SpawnLocation,
				FRotator::ZeroRotator,
				SpawnParams);

			NewWeaponActor->SetActorScale3D(Spec.Scale);
			if (!IsValid(NewWeaponActor))
				continue;

			NewWeaponActor->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

			//빙글뱅글 돌리기
			if (IsValid(NewWeaponActor))
			{
				auto* Rot = NewObject<URotatingMovementComponent>(NewWeaponActor, TEXT("Rotator"));
				Rot->RotationRate = FRotator(0.f, 120.f, 0.f);
				Rot->bRotationInLocalSpace = true;
				Rot->SetUpdatedComponent(NewWeaponActor->GetRootComponent());
				Rot->RegisterComponent();
			}

			if (APickup_Weapon* PW = Cast<APickup_Weapon>(NewWeaponActor))
			{
				PW->OnConsumed.AddUObject(this, &ATileBase::HandlePickUpConsumed);
			}
			SpawnedWeapons.Add(NewWeaponActor);
			++Spawned;
		}
	}
}

void ATileBase::RebuildInstances()
{
	GenerateInstances();
}

void ATileBase::HandlePickUpConsumed(APickup_Weapon* Self)
{
	if (!HasAuthority() || !IsValid(Self))
		return;

	UClass* SameClass = Self->GetClass();

	TArray<AActor*> SnapShot = SpawnedWeapons;

	for (AActor* Actor : SnapShot)
	{
		APickup_Weapon* PW = Cast<APickup_Weapon>(Actor);
		if (!IsValid(PW) || PW == Self) continue;

		if (PW->GetOwner() != this)
			continue;

		if (PW->GetClass() == SameClass)
		{
			PW->Destroy();
		}
	}

	SpawnedWeapons.RemoveAll([](AActor* Actor){ return !IsValid(Actor); });
}

#if WITH_EDITOR
void ATileBase::PrintIndex()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("Tile Index: %d"), TileIndex));
}
#endif
