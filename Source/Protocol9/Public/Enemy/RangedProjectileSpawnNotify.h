// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RangedMonsterBase.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "RangedProjectileSpawnNotify.generated.h"

/**
 * 
 */
UCLASS()
class PROTOCOL9_API URangedProjectileSpawnNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
