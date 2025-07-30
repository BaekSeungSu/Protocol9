// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MonsterBase.h"
#include "MeleeMonsterBase.generated.h"

/**
 * 
 */
UCLASS()
class PROTOCOL9_API AMeleeMonsterBase : public AMonsterBase
{
	GENERATED_BODY()

	virtual void PerformAttack() override;
};
