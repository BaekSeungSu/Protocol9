#include "Weapons/ProjectilePool.h"

#include "Kismet/GameplayStatics.h"
#include "Weapons/BaseProjectile.h"

AProjectilePool::AProjectilePool()
{
	PrimaryActorTick.bCanEverTick = false;
	InitialSize = 32;
	bAutoExpand = true;
	MaxSize = 0;
	ExpandStep = 10;
	bAttachPooledAsChild = true;
}

void AProjectilePool::BeginPlay()
{
	Super::BeginPlay();
	LogConfigErrorsIfAny();
	if (!ProjectileClass) return;
	
	Prewarm(InitialSize);
}


ABaseProjectile* AProjectilePool::Acquire(const FVector& SpawnLocation, const FRotator& SpawnRotation,
	const FVector& InitialVelocity, AActor* InOwner, APawn* InInstigator)
{
	if (!ProjectileClass) return nullptr;
	if (InactiveProjectiles.Num() == 0)
	{
		const bool bExpanded = TryExpand(1);
		if (!bExpanded)
		{
			UE_LOG(LogTemp, Error, TEXT("No more projectiles available in pool!"));
			return nullptr;
		}
	}

	ABaseProjectile* Projectile = InactiveProjectiles.Pop();
	if (!Projectile) return nullptr;

	ActiveProjectiles.Add(Projectile);
	Projectile->ActivateProjectile(SpawnLocation, SpawnRotation, InitialVelocity, InOwner, InInstigator);

	return Projectile;
}

void AProjectilePool::Release(ABaseProjectile* Projectile)
{
	if (!Projectile) return;
	if (!ActiveProjectiles.Contains(Projectile))
	{
		Projectile->DeactivateProjectile();
		return;
	}

	Projectile->DeactivateProjectile();
	Projectile->ResetProjectile();

	ActiveProjectiles.Remove(Projectile);
	InactiveProjectiles.Add(Projectile);
}


void AProjectilePool::Prewarm(int32 Count)
{
	for (int32 i = 0; i < Count; i++)
	{
		if (ABaseProjectile* Projectile = SpawnOne())
		{
			Projectile->DeactivateProjectile();
			InactiveProjectiles.Add(Projectile);
		}
	}
}

bool AProjectilePool::TryExpand(int32 NeededCount)
{
	if (!bAutoExpand) return false;
	const int32 CurrentCount = InactiveProjectiles.Num() + ActiveProjectiles.Num();
	if (MaxSize > 0 && CurrentCount >= MaxSize) return false;

	const int32 ToCreate = (MaxSize > 0) ? FMath::Min(ExpandStep, MaxSize - CurrentCount) : ExpandStep;

	int32 CreatedCount = 0;
	for (int32 i = 0; i < ToCreate; i++)
	{
		if (ABaseProjectile* Projectile = SpawnOne())
		{
			Projectile->DeactivateProjectile();
			InactiveProjectiles.Add(Projectile);
			CreatedCount++;
		}
		else
		{
			break;
		}
	}

	if (CreatedCount > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("Expanded projectile pool to %d"), CurrentCount + CreatedCount);
	}
	return InactiveProjectiles.Num() >= NeededCount;
}

ABaseProjectile* AProjectilePool::SpawnOne()
{
	if (!ProjectileClass) return nullptr;
	if (MaxSize > 0 && (InactiveProjectiles.Num() + ActiveProjectiles.Num()) >= MaxSize)
	{
		UE_LOG(LogTemp, Error, TEXT("No more projectiles available in pool!"));
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	const FTransform T(GetActorRotation(), GetActorLocation());
	ABaseProjectile* NewProjectile = GetWorld()->SpawnActorDeferred<ABaseProjectile>(ProjectileClass, T, this);
	if (!NewProjectile) return nullptr;

	NewProjectile->SetOwningPool(this);

	UGameplayStatics::FinishSpawningActor(NewProjectile, T);

	if (bAttachPooledAsChild)
	{
		NewProjectile->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	}
	return NewProjectile;
}

void AProjectilePool::LogConfigErrorsIfAny() const
{
	if (!ProjectileClass)
	{
		UE_LOG(LogTemp, Error, TEXT("ProjectileClass is not set!"));
	}
	if (MaxSize > 0 && InitialSize > MaxSize)
	{
		UE_LOG(LogTemp, Error, TEXT("InitialSize is larger than MaxSize!"));
	}
}




