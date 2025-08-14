#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BossPattern2SweepNotifyState.generated.h"

UCLASS()
class PROTOCOL9_API UBossPattern2SweepNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sweep")
	FVector BoxHalfSize = FVector(400.f, 600.f, 500.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sweep")
	float SweepSpeed = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sweep")
	float Damage = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sweep")
	FVector Direction = FVector(0.0f,-1.0f,0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sweep")
	FVector BoxCenterOffset = FVector(0.0f,0.0f,0.0f);
private:
	FVector Origin;
	
	TSet<TWeakObjectPtr<AActor>> HitActors;
	float AccumulatedDistance = 0.f;
};
