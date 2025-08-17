#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BossPattern3RushNotifyState.generated.h"

UCLASS()
class PROTOCOL9_API UBossPattern3RushNotifyState : public UAnimNotifyState
{
    GENERATED_BODY()

public:
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                              float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
    virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                             float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                            const FAnimNotifyEventReference& EventReference) override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rush")
    float SpeedMultiplier = 4.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rush")
    float ArriveThreshold = 150.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hit")
    FVector HitBoxHalfSize = FVector(400.f, 400.f, 400.f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hit")
    float HitDamage = 25.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hit")
    float KnockbackStrength = 1200.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hit")
    float KnockUpStrength = 400.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hit")
    bool bPreventRepeatHitDuringState = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hit|VFX")
    UParticleSystem* HitImpactFX = nullptr;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hit|VFX")
    FVector HitFXScale = FVector(1.f, 1.f, 1.f);
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hit|VFX")
    bool bAlignFXToImpactNormal = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hit")
    float HitInterval = 0.5f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Hit")
    float HitTimer = 0.f;
    
private:
    FVector TargetLocation;
    float OriginalMaxWalkSpeed = 0.f;
    TSet<TWeakObjectPtr<AActor>> HitActors;
};
