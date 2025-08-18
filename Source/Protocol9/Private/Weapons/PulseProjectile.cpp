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
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
	

	const FVector ExplosionOrigin = Hit.ImpactPoint;
	
	if (OtherActor && OtherActor != this && OtherActor != GetOwner())
	{
		TArray<AActor*> IgnoreActors;
		if (AActor* MyOwner = GetOwner())
		{
			IgnoreActors.Add(MyOwner);
		}
		if (APawn* Inst = GetInstigator())
		{
			IgnoreActors.Add(Inst);
		}
		IgnoreActors.Add(this);
		UGameplayStatics::ApplyRadialDamage(GetWorld(), Damage, ExplosionOrigin, DamageRadius, UDamageType::StaticClass(), IgnoreActors, this, GetInstigatorController(), false, ECC_Visibility);
	}

	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, GetActorLocation());
	}
	DrawDebugSphere(GetWorld(), GetActorLocation(), DamageRadius, 10, FColor::Red, false, 1.0f);	
	Destroy();
}
