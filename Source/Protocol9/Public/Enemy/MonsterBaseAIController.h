#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MonsterBaseAIController.generated.h"

class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
class UAIPerceptionComponent;
class AMonsterBase;
UCLASS()
class PROTOCOL9_API AMonsterBaseAIController : public AAIController
{
	GENERATED_BODY()

public:
	AMonsterBaseAIController();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAIPerceptionComponent* AIPerceptionComponent;
	UPROPERTY()
	UAISenseConfig_Sight* SightConfig;
	UPROPERTY()
	UAISenseConfig_Hearing* HearingConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Perception")
	float SightRadius = 2500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Perception")
	float HearingRadius = 1500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Perception")
	float PeripheralVisionAngleDegrees = 180.0f;
protected:
	virtual void BeginPlay() override;
	UPROPERTY()
	AMonsterBase* Monster;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class UBlackboardComponent* BlackboardComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class UBehaviorTreeComponent* BehaviorComp;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UBehaviorTree* BehaviorTree;
	
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};
