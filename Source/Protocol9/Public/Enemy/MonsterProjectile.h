#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "MonsterProjectile.generated.h"

UCLASS()
class PROTOCOL9_API AMonsterProjectile : public AActor
{
    GENERATED_BODY()

public:
    AMonsterProjectile();

protected:
    virtual void BeginPlay() override;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class USphereComponent* CollisionComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
    class UParticleSystemComponent* ParticleSystemComponent;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UProjectileMovementComponent* ProjectileMovement;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Settings")
    float ProjectileLifetime = 8.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
    float ExplosionRadius = 400.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
    float ExplosionDamage = 40.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
    float MaxDamageRadius = 150.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
    TSubclassOf<class UDamageType> ExplosionDamageType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    class UParticleSystem* ExplosionEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    class USoundBase* ExplosionSound;

    UPROPERTY()
    bool bHasExploded = false;

    UPROPERTY()
    FTimerHandle LifetimeTimerHandle;

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, 
               UPrimitiveComponent* OtherComp, FVector NormalImpulse, 
               const FHitResult& Hit);

    void Explode(FVector ExplosionLocation);
    void AddNearbyMonstersToIgnoreList(TArray<AActor*>& IgnoredActors, FVector ExplosionCenter);
    void OnLifetimeExpired();

public:
    UFUNCTION(BlueprintCallable, Category = "Projectile")
    void FireAtTarget(FVector TargetLocation, float ArcHeight = 350.0f);
};