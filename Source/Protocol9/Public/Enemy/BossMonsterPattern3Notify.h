#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "BossMonsterPattern3Notify.generated.h"

/**
 * 
 */
UCLASS()
class PROTOCOL9_API UBossMonsterPattern3Notify : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	UBossMonsterPattern3Notify();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	FVector BoxTraceHalfSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	FVector BoxCenterOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float Damage;
};
