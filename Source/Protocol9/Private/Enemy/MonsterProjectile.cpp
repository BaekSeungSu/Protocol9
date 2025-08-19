// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/MonsterProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "TimerManager.h"
#include "Enemy/MonsterBase.h"
#include "Engine/Engine.h"
#include "Engine/OverlapResult.h"
#include "DrawDebugHelpers.h"

AMonsterProjectile::AMonsterProjectile()
{
    PrimaryActorTick.bCanEverTick = false;

    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
    RootComponent = CollisionComponent;
    CollisionComponent->SetSphereRadius(3.0f);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    CollisionComponent->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel4); 
    
    CollisionComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel4, ECR_Ignore);
    CollisionComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECR_Ignore);
    
    ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComponent"));
    ParticleSystemComponent->SetupAttachment(CollisionComponent);
    
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;

    ProjectileMovement->bInitialVelocityInLocalSpace = false;

    CollisionComponent->OnComponentHit.AddDynamic(this, &AMonsterProjectile::OnHit);
    
    bHasExploded = false;
}

void AMonsterProjectile::BeginPlay()
{
    Super::BeginPlay();
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        if (APawn* PlayerPawn = PC->GetPawn())
        {
            CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
            
        }
    }

    GetWorld()->GetTimerManager().SetTimer(
        LifetimeTimerHandle,
        this,
        &AMonsterProjectile::OnLifetimeExpired,
        ProjectileLifetime,
        false
    );
}

void AMonsterProjectile::FireAtTarget(FVector TargetLocation, float ArcHeight)
{
    if (AActor* ProjectileOwner = GetOwner())
    {
        CollisionComponent->IgnoreActorWhenMoving(ProjectileOwner, true);
    }
    
    FVector StartLocation = GetActorLocation();

    //FVector LaunchVelocity;
    // bool bValidTrajectory = UGameplayStatics::SuggestProjectileVelocity_CustomArc(
    //     GetWorld(),
    //     LaunchVelocity,
    //     StartLocation,
    //     TargetLocation,
    //     GetWorld()->GetGravityZ(),
    //     ArcHeight
    // );
    //
    // if (bValidTrajectory)
    // {
    //     ProjectileMovement->Velocity = LaunchVelocity;
    // }
    // else
    // {
    FVector Direction = (TargetLocation - StartLocation).GetSafeNormal();
    ProjectileMovement->Velocity = Direction * ProjectileMovement->InitialSpeed;
    //}
}

void AMonsterProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                              const FHitResult& Hit)
{
    if (bHasExploded) return;

    if (OtherActor && OtherActor != this && OtherActor != GetOwner())
    {
        Explode(Hit.Location);
    }
}

void AMonsterProjectile::Explode(FVector ExplosionLocation)
{
    if (bHasExploded) return;
    bHasExploded = true;
    
    if (ExplosionEffect)
    {
        UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(), 
            ExplosionEffect, 
            ExplosionLocation, 
            FRotator::ZeroRotator
        );
    }

    if (ExplosionSound)
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, ExplosionLocation);
    }

    TArray<AActor*> IgnoredActors;
    IgnoredActors.Add(this);
    if (GetOwner())
    {
        IgnoredActors.Add(GetOwner());
    }
    
    AddNearbyMonstersToIgnoreList(IgnoredActors, ExplosionLocation);

    //DrawDebugSphere(GetWorld(), ExplosionLocation, ExplosionRadius, 32, FColor::Red, false, 3.0f, 0, 2.0f );
    
    UGameplayStatics::ApplyRadialDamageWithFalloff(
        GetWorld(),
        ExplosionDamage,        
        ExplosionDamage * 0.3f,
        ExplosionLocation,     
        MaxDamageRadius,        
        ExplosionRadius,    
        1.0f,                   
        ExplosionDamageType,   
        IgnoredActors,          
        this,                  
        GetInstigatorController(),
        ECollisionChannel::ECC_Visibility
    );

    // if (GEngine && GEngine->bEnableOnScreenDebugMessages)
    // {
    //     DrawDebugSphere(GetWorld(), ExplosionLocation, ExplosionRadius, 32, 
    //                    FColor::Red, false, 3.0f, 0, 2.0f);
    //     DrawDebugSphere(GetWorld(), ExplosionLocation, MaxDamageRadius, 16, 
    //                    FColor::Orange, false, 3.0f, 0, 2.0f);
    // }

    if (LifetimeTimerHandle.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(LifetimeTimerHandle);
    }
    Destroy();
}

void AMonsterProjectile::AddNearbyMonstersToIgnoreList(TArray<AActor*>& IgnoredActors, FVector ExplosionCenter)
{
    TArray<FOverlapResult> OverlapResults;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);
    if (GetOwner())
    {
        QueryParams.AddIgnoredActor(GetOwner());
    }

    bool bHasOverlap = GetWorld()->OverlapMultiByChannel(
        OverlapResults,
        ExplosionCenter,
        FQuat::Identity,
        ECollisionChannel::ECC_Pawn,
        FCollisionShape::MakeSphere(ExplosionRadius+20.0f),
        QueryParams
    );

    if (bHasOverlap)
    {
        for (const FOverlapResult& Result : OverlapResults)
        {
            if (AMonsterBase* Monster = Cast<AMonsterBase>(Result.GetActor()))
            {
                IgnoredActors.Add(Monster);

            }
        }
    }
}

void AMonsterProjectile::OnLifetimeExpired()
{
    if (!bHasExploded)
    {
        Explode(GetActorLocation());
    }
}

