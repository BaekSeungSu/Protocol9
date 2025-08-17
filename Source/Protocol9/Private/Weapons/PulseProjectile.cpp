#include "Weapons/PulseProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

APulseProjectile::APulseProjectile()
{
	ProjectileMovement->InitialSpeed = 5000.f;
	ProjectileMovement->MaxSpeed = 5000.f;
	ProjectileMovement->ProjectileGravityScale = 0.5f;

	HitSound = nullptr;
	DamageRadius = 300.0f;
}

void APulseProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                             FVector NormalImpulse, const FHitResult& Hit)
{
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, GetActorLocation());
	}
	DrawDebugSphere(GetWorld(), GetActorLocation(), DamageRadius, 10, FColor::Red, false, 1.0f);	
	
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}

void APulseProjectile::ApplyDamageOnHit(const FHitResult& HitResult)
{
	const FVector Origin = HitResult.ImpactPoint;

	TArray<AActor*> IgnoreActors;
	if (AActor* MyOwner = GetOwner())
	{
		IgnoreActors.Add(MyOwner);
	}
	if (APawn* Instig = GetInstigator())
	{
		IgnoreActors.Add(Instig);
	}
	IgnoreActors.Add(this);
	UGameplayStatics::ApplyRadialDamage(
		GetWorld(),
		Damage,
		Origin,
		DamageRadius,
		UDamageType::StaticClass(),
		IgnoreActors,
		this,
		GetInstigatorController(),
		false,
		ECC_Visibility
		);
	
}
