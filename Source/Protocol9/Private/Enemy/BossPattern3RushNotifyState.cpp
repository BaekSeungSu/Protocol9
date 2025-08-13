#include "Enemy/BossPattern3RushNotifyState.h"
#include "Enemy/BossMonsterBase.h"
#include "Character/MainCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/PrimitiveComponent.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

void UBossPattern3RushNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                               float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    if (!MeshComp) return;
    ABossMonsterBase* Boss = Cast<ABossMonsterBase>(MeshComp->GetOwner());
    if (!Boss) return;
    
    if (APawn* Player = UGameplayStatics::GetPlayerPawn(Boss, 0))
    {
        TargetLocation = Player->GetActorLocation();
    }
    
    if (UCharacterMovementComponent* Move = Boss->FindComponentByClass<UCharacterMovementComponent>())
    {
        OriginalMaxWalkSpeed = Move->MaxWalkSpeed;
        Move->MaxWalkSpeed = Boss->WalkSpeed * SpeedMultiplier;
    }
    
    HitActors.Empty();
}

void UBossPattern3RushNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                              float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
    HitTimer+= FrameDeltaTime;
    if (HitTimer >= HitInterval)
    {
        HitTimer = 0.f;
        HitActors.Empty();
    }
    if (!MeshComp) return;
    ABossMonsterBase* Boss = Cast<ABossMonsterBase>(MeshComp->GetOwner());
    if (!Boss) return;
    
    APawn* Player = UGameplayStatics::GetPlayerPawn(Boss, 0);
    if (!Player) return;

    const FVector BossLocation = Boss->GetActorLocation();
    TargetLocation = Player->GetActorLocation();
    const FVector Dir = (TargetLocation - BossLocation).GetSafeNormal();
    
    Boss->AddMovementInput(Dir, 1.0f);
    
    const FVector BoxCenter = BossLocation + Dir * (HitBoxHalfSize.X);
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjTypes;
    ObjTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

    TArray<AActor*> Ignore{Boss};
    TArray<AActor*> Overlaps;

    const bool bHit = UKismetSystemLibrary::BoxOverlapActors(
        Boss, BoxCenter, HitBoxHalfSize, ObjTypes, nullptr, Ignore, Overlaps
    );
    
    DrawDebugBox(
        Boss->GetWorld(),
        BoxCenter,
        HitBoxHalfSize,
        Boss->GetActorRotation().Quaternion(),
        bHit ? FColor::Red : FColor::Green,
        false, 0.05f, 0, 2.0f
    );

    if (!bHit) return;
    
    for (AActor* Actor : Overlaps)
    {
        if (!Actor)
            continue;
        if (bPreventRepeatHitDuringState && HitActors.Contains(Actor))
            continue;
        
        UGameplayStatics::ApplyDamage
        (
            Actor, HitDamage*Boss->DamageModifier, Boss->GetController(), Boss, UDamageType::StaticClass()
        );
        
        if (AMainCharacter* Char = Cast<AMainCharacter>(Actor))
        {
            const FVector KnockDir = (Actor->GetActorLocation() - BossLocation).GetSafeNormal();
            const FVector LaunchVel = KnockDir * KnockbackStrength + FVector::UpVector * KnockUpStrength;
            Char->LaunchCharacter(LaunchVel, true, true);
        }
        
        if (HitImpactFX)
        {
            const FVector BossLoc = Boss->GetActorLocation();
            const FVector VictimLoc = Actor->GetActorLocation();
            const FVector DirToVictim = (VictimLoc - BossLocation).GetSafeNormal();
        
            const float TraceLength = FVector::Dist(BossLocation, VictimLoc) + 100.f;
            const FVector TraceStart = BossLocation + DirToVictim * 30.f;
            const FVector TraceEnd   = TraceStart + DirToVictim * TraceLength;
        
            TArray<AActor*> ActorsToIgnore;
            FCollisionQueryParams Params(FName(TEXT("")), false, Actor);
            Params.AddIgnoredActor(Actor);

            FHitResult HitResult;
            const bool bImpact = GetWorld()->LineTraceSingleByChannel(
                HitResult,
                TraceStart,
                TraceEnd,
                ECC_Pawn,
                Params,
                FCollisionResponseParams::DefaultResponseParam
            );

            FVector HitLocation;
            FRotator HitRotation = FRotator::ZeroRotator;

            if (bImpact && HitResult.bBlockingHit)
            {
                HitLocation = HitResult.ImpactPoint;

                if (bAlignFXToImpactNormal)
                {
                    HitRotation = HitResult.ImpactNormal.Rotation();
                }
                else
                {
                    HitRotation = DirToVictim.Rotation();
                }
            }
            else
            {
                HitLocation = (BossLocation + VictimLoc) * 0.5f;
                HitRotation = DirToVictim.Rotation();
            }
        
            UGameplayStatics::SpawnEmitterAtLocation(
                Boss->GetWorld(),
                HitImpactFX,
                FTransform(HitRotation, HitLocation, HitFXScale),
                true
            );
        }
        if (bPreventRepeatHitDuringState)
        {
            HitActors.Add(Actor);
        }
    }
}

void UBossPattern3RushNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                             const FAnimNotifyEventReference& EventReference)
{
    if (!MeshComp)
        return;
    ABossMonsterBase* Boss = Cast<ABossMonsterBase>(MeshComp->GetOwner());
    if (!Boss)
        return;
    
    if (UCharacterMovementComponent* Move = Boss->FindComponentByClass<UCharacterMovementComponent>())
    {
        if (OriginalMaxWalkSpeed > 0.f)
        {
            Move->MaxWalkSpeed = OriginalMaxWalkSpeed;
        }
    }

    HitActors.Empty();
}
