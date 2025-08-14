// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "BossAttackPatternDataRow.generated.h"
UENUM(BlueprintType)
enum class EBossAttackPattern : uint8
{
	Pattern1 UMETA(DisplayName = "Pattern1"),
	Pattern2 UMETA(DisplayName = "Pattern2"),
	Pattern3 UMETA(DisplayName = "Pattern3")
};

UENUM(BlueprintType)
enum class EBossPhase : uint8
{
	Phase1 UMETA(DisplayName = "Phase1"),
	Phase2 UMETA(DisplayName = "Phase2")
};

USTRUCT(BlueprintType)
struct PROTOCOL9_API FBossAttackPatternDataRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern")
	FName PatternName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimMontage")
	TObjectPtr<class UAnimMontage> AnimMontage;
};
