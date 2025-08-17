// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "FlyingMonsterBase.h"
#include "FlyingProjectileSpawnNotify.generated.h"

/**
 * 
 */
UCLASS()
class PROTOCOL9_API UFlyingProjectileSpawnNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
