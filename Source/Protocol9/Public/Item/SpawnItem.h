#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SpawnItem.generated.h"

USTRUCT(BlueprintType)
struct FSpawnItem: public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AItemBase> SpawnItem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnChance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PoolSize;
};
