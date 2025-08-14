// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "BossMonsterPattern2Notify.generated.h"

/**
 * 
 */
UCLASS()
class PROTOCOL9_API UBossMonsterPattern2Notify : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	UBossMonsterPattern2Notify();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	FVector BoxTraceHalfSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	FVector BoxCenterOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float Damage;
};
