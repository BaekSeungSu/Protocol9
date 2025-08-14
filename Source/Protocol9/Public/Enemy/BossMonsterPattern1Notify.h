// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "BossMonsterPattern1Notify.generated.h"

/**
 * 
 */
UCLASS()
class PROTOCOL9_API UBossMonsterPattern1Notify : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	UBossMonsterPattern1Notify();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	FVector BoxTraceHalfSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float Damage;
};
