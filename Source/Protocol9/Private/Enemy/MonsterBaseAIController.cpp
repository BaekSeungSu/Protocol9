#include "Enemy/MonsterBaseAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"

AMonsterBaseAIController::AMonsterBaseAIController()
{
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoardComp"));
	BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
}

void AMonsterBaseAIController::BeginPlay()
{
	Super::BeginPlay();

	if (Blackboard && BehaviorTree)
	{
		UseBlackboard(BehaviorTree->BlackboardAsset,BlackboardComp);
		RunBehaviorTree(BehaviorTree);
	}
}

