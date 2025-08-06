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

	

public:
	UFUNCTION()
	void AttackCheck();

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="MeleeAttack")
	FVector AttackBoxHalfSize = {100.0f, 50.0f, 50.0f};

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= "MeleeAttack")
	FVector AttackBoxOffset = {0.0f, 0.0f, 0.0f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeleeAttack")
	float AttackDamage = 20.0f;
};
