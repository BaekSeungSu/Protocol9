#include "Item/PoolingManager.h"
#include "Enemy/MonsterBase.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy/MonsterSpawner.h"
#include "Item/ObjectPoolingComponent.h"


APoolingManager::APoolingManager()
{
 	
	PrimaryActorTick.bCanEverTick = false;
	ObjectPoolingComponent = CreateDefaultSubobject<UObjectPoolingComponent>(TEXT("ObjectPoolingComponent"));
	
}


void APoolingManager::BeginPlay()
{
	Super::BeginPlay();


	//��� ���͸� ���� �迭 
	TArray<AActor*> FoundMonsters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMonsterSpawner::StaticClass(), FoundMonsters);
	for (AActor* MonsterActor : FoundMonsters)
	{
		AMonsterSpawner* Spawner = Cast<AMonsterSpawner>(MonsterActor);
		if (Spawner)
		{
			// �����ʰ� ���͸� ������ ������ OnMonsterSpawned �Լ��� ȣ���ϵ��� ���ε��մϴ�.
			Spawner->OnMonsterSpawned.AddDynamic(this, &APoolingManager::OnMonsterSpawned);
		}
	}
}

//���Ͱ� ���ο� ���͸� �����Ҷ����� �� �Լ��� ȣ���
void APoolingManager::OnMonsterSpawned(AMonsterBase* NewMonster)
{
	if (NewMonster)
	{
		// ���� ������ ������ ���� ��������Ʈ�� �츮�� �Լ��� ���ε��մϴ�.
		NewMonster->OnMonsterDeadLocation.AddDynamic(this, &APoolingManager::MonsterDeathLocation);
	}
}


void APoolingManager::MonsterDeathLocation(FVector Location)
{
	UObjectPoolingComponent* PoolingComp = FindComponentByClass<UObjectPoolingComponent>();
	if (!PoolingComp) return;
	
	int32 RandValue = FMath::RandRange(1, 100);
	if (RandValue<=20)
	{
		PoolingComp -> SpawnRandomItem(Location);
	}
}

