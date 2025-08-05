// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MonsterSpawnRow.generated.h"
USTRUCT(BlueprintType)
struct FMonsterSpawnRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	FName MonsterName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	TSubclassOf<class AMonsterBase> MonsterClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	int32 MonsterMaxCount;
};
