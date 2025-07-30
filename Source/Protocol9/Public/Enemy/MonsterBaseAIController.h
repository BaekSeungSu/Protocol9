#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MonsterBaseAIController.generated.h"

UCLASS()
class PROTOCOL9_API AMonsterBaseAIController : public AAIController
{
	GENERATED_BODY()

public:
	AMonsterBaseAIController();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class UBlackboardComponent* BlackboardComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class UBehaviorTreeComponent* BehaviorComp;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UBehaviorTree* BehaviorTree;
};
