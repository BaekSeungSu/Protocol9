#include "Enemy/MonsterBaseAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
//#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Enemy/MonsterBase.h"

AMonsterBaseAIController::AMonsterBaseAIController()
{
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoardComp"));
	BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
	
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = SightRadius;
	SightConfig->LoseSightRadius = SightRadius+500.0f;
	SightConfig->PeripheralVisionAngleDegrees = PeripheralVisionAngleDegrees;
	SightConfig->SetMaxAge(1.0f);
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
	
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	HearingConfig->HearingRange = HearingRadius;
	HearingConfig->SetMaxAge(1.0f);
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = false;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = false;
	
	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
	AIPerceptionComponent->ConfigureSense(*HearingConfig);
}

void AMonsterBaseAIController::BeginPlay()
{
	Super::BeginPlay();

	if (Blackboard && BehaviorTree)
	{
		UseBlackboard(BehaviorTree->BlackboardAsset,BlackboardComp);
		RunBehaviorTree(BehaviorTree);
	}
	if (AIPerceptionComponent)
	{
		AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AMonsterBaseAIController::OnTargetPerceptionUpdated);
	}
	Monster = Cast<AMonsterBase>(GetPawn());
}

void AMonsterBaseAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	FString SenseType = TEXT("Unknown");
    
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
		SenseType = TEXT("SIGHT");
	else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
		SenseType = TEXT("HEARING");
    
	UE_LOG(LogTemp, Warning, TEXT("Perception: %s detected by %s"), 
		   Actor ? *Actor->GetName() : TEXT("NULL"), *SenseType);
	
	APlayerController* PlayerController = Cast<APlayerController>(Cast<APawn>(Actor) ? Cast<APawn>(Actor)->GetController() : nullptr);
	if (PlayerController == nullptr) return;

	if (Stimulus.WasSuccessfullySensed())
	{
		Monster->SetTargetPlayer(Cast<APawn>(Actor));
		if (Monster->CurrentState != EMonsterState::Attacking)
		{
			Monster->SetState(EMonsterState::Chasing);
		}
	}
	else
	{
		Monster->SetTargetPlayer(nullptr);
		Monster->SetState(EMonsterState::Returning);
	}
}