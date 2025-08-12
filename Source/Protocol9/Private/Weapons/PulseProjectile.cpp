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
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1,
			2.0f,
			FColor::Red,
			FString::Printf(TEXT("Hit!")));
	}
	if (OtherActor && OtherActor != this && OtherActor != GetOwner())
	{
		TArray<AActor*> IgnoreActors;
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			IgnoreActors.Add(MyOwner);
		}
		IgnoreActors.Add(this);
		UGameplayStatics::ApplyRadialDamage(GetWorld(), Damage, GetVelocity().GetSafeNormal(), DamageRadius, UDamageType::StaticClass(), IgnoreActors, this, GetInstigatorController(), false, ECC_Visibility);
	}

	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, GetActorLocation());
	}
	DrawDebugSphere(GetWorld(), GetActorLocation(), DamageRadius, 10, FColor::Red, false, 1.0f);	
	Destroy();
}
