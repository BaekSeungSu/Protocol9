#include "Enemy/BossPattern2SweepNotifyState.h"
#include "Enemy/BossMonsterBase.h"
#include "Character/MainCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

void UBossPattern2SweepNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    if (!MeshComp) return;
    ABossMonsterBase* Boss = Cast<ABossMonsterBase>(MeshComp->GetOwner());
    if (!Boss) return;

    Origin = Boss->GetActorLocation();
    AccumulatedDistance = 0.f;
    HitActors.Empty();

}

void UBossPattern2SweepNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
    if (!MeshComp) return;
    ABossMonsterBase* Boss = Cast<ABossMonsterBase>(MeshComp->GetOwner());
    if (!Boss) return;
    
    float Step = SweepSpeed * FrameDeltaTime;
    AccumulatedDistance += Step;

    FVector BoxCenter = Origin + Direction * AccumulatedDistance + BoxCenterOffset;
    
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjTypes;
    ObjTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

    TArray<AActor*> Ignore; 
    Ignore.Add(Boss);

    TArray<AActor*> Overlapped;
    bool bHit = UKismetSystemLibrary::BoxOverlapActors(
        Boss,
        BoxCenter,
        BoxHalfSize,
        ObjTypes,
        nullptr,
        Ignore,
        Overlapped
    );
    
    // DrawDebugBox(
    //     Boss->GetWorld(),
    //     BoxCenter,
    //     BoxHalfSize,
    //     Boss->GetActorRotation().Quaternion(),
    //     bHit ? FColor::Red : FColor::Green,
    //     false,
    //     0.1f,
    //     0,
    //     2.0f
    // );
    
    if (bHit)
    {
        for (AActor* Actor : Overlapped)
        {
            AMainCharacter* Player = Cast<AMainCharacter>(Actor);
            if (Player && !HitActors.Contains(Actor))
            {
                UGameplayStatics::ApplyDamage(
                    Player,
                    Damage,
                    Boss->GetController(),
                    Boss,
                    UDamageType::StaticClass()
                );
                HitActors.Add(Actor);
            }
        }
    }
}

void UBossPattern2SweepNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    HitActors.Empty();
}
