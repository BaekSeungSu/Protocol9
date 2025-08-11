#pragma once

#include "CoreMinimal.h"
#include "LevelUpRow.generated.h"

USTRUCT(BlueprintType)
struct FLevelUpRow: public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Value;
};
