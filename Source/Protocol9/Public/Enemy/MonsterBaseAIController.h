#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MonsterBaseAIController.generated.h"

class AMonsterBase;
UCLASS()
class PROTOCOL9_API AMonsterBaseAIController : public AAIController
{
	GENERATED_BODY()

public:
	AMonsterBaseAIController();

protected:
	virtual void BeginPlay() override;
};
