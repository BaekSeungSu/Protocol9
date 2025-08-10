#include "Weapons/BaseProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"


ABaseProjectile::ABaseProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	ProjectileCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ProjectileCollision"));
	SetRootComponent(ProjectileCollision);
	ProjectileCollision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
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
	InitialLifeSpan = 3.0f;
	
}

void ABaseProjectile::FireInDirection(const FVector& ShootDirection)
{
	ProjectileEffect->Activate(true);
	ProjectileMovement->Velocity = ShootDirection *	ProjectileMovement->InitialSpeed;
}

void ABaseProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                            FVector NormalImpulse, const FHitResult& Hit)
{
	ProjectileMovement->StopMovementImmediately();
	ProjectileEffect->Deactivate();
}



