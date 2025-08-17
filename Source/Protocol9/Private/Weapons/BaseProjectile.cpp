#include "Weapons/BaseProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Weapons/ProjectilePool.h"

ABaseProjectile::ABaseProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	ProjectileCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ProjectileCollision"));
	SetRootComponent(ProjectileCollision);
	ProjectileCollision->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
	ProjectileCollision->InitSphereRadius(10.0f);
	ProjectileCollision->OnComponentHit.AddDynamic(this, &ABaseProjectile::OnHit);

	ProjectileEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileEffect"));
	ProjectileEffect->SetupAttachment(GetRootComponent());
	ProjectileEffect->bAutoActivate = false;
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->SetUpdatedComponent(ProjectileCollision);
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	
	Damage = 12.0f;
	InitialLifeSpan = 0.0f;
	
}

void ABaseProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseProjectile::FireInDirection(const FVector& ShootDirection)
{
	if (ProjectileEffect)
	{
		ProjectileEffect->Activate(true);
	}
	if (ProjectileMovement)
	{
		ProjectileMovement->Velocity = ShootDirection *	ProjectileMovement->InitialSpeed;
	}
}

void ABaseProjectile::ActivateProjectile(const FVector& SpawnLocation, const FRotator& SpawnRotation, const FVector& Velocity, AActor* InOwner, APawn* InInstigator)
{
	SetActorLocationAndRotation(SpawnLocation, SpawnRotation, false, nullptr, ETeleportType::TeleportPhysics);
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);

	if (InOwner) SetOwner(InOwner);
	if (InInstigator) SetInstigator(InInstigator);

	if (APawn* Instig = GetInstigator())
	{
		ProjectileCollision->IgnoreActorWhenMoving(Instig, true);
	}

	if (AActor* OwnerActor = GetOwner())
	{
		ProjectileCollision->IgnoreActorWhenMoving(OwnerActor, true);
	}
	
	if (ProjectileMovement)
	{
		ProjectileMovement->StopMovementImmediately();
		ProjectileMovement->Velocity = Velocity;
		ProjectileMovement->SetUpdatedComponent(ProjectileCollision);
		ProjectileMovement->Activate(true);
		ProjectileMovement->UpdateComponentVelocity();
	}

	if (ProjectileEffect)
	{
		ProjectileEffect->Activate(true);
	}

	GetWorldTimerManager().ClearTimer(LifeTimerHandle);
	GetWorldTimerManager().SetTimer(LifeTimerHandle, [this]()
	{
		if (OwningPool)
		{
			OwningPool->Release(this);
		}
		else
		{
			Destroy();
		}
	}, LifeTime, false);
	OnActivated();
}

void ABaseProjectile::DeactivateProjectile()
{
	GetWorldTimerManager().ClearTimer(LifeTimerHandle);
	
	if (ProjectileMovement)
	{
		ProjectileMovement->StopMovementImmediately();
		ProjectileMovement->Deactivate();
	}
	if (ProjectileEffect)
	{
		ProjectileEffect->DeactivateImmediate();
		ProjectileEffect->SetHiddenInGame(true);
	}

	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);

	OnDeactivated();
}

void ABaseProjectile::ResetProjectile()
{
	// 다음 사용을 위한 내부 상태 초기화
	if (ProjectileMovement)
	{
		ProjectileMovement->Velocity = FVector::ZeroVector;
	}

	OnReset();
}


void ABaseProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                            FVector NormalImpulse, const FHitResult& Hit)
{
	if (ProjectileMovement)
	{
		ProjectileMovement->StopMovementImmediately();
	}
	if (ProjectileEffect)
	{
		ProjectileEffect->Deactivate();
	}

	ApplyDamageOnHit(Hit);
	
	if (HitEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
	}

	if (OwningPool)
	{
		OwningPool->Release(this);
	}
	else
	{
		Destroy();
	}
}

void ABaseProjectile::ApplyDamageOnHit(const FHitResult& HitResult)
{
	AActor* HitActor = HitResult.GetActor();
	if (!HitActor || HitActor == GetOwner()) return;

	AController* IntigatorController = GetInstigator() ? GetInstigator()->GetController() : nullptr;

	UGameplayStatics::ApplyPointDamage(HitActor, Damage, GetVelocity().GetSafeNormal(), HitResult, IntigatorController, this, UDamageType::StaticClass());
}
