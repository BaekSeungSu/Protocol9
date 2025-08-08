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


	//모든 몬스터를 담을 배열 
	TArray<AActor*> FoundMonsters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMonsterSpawner::StaticClass(), FoundMonsters);
	for (AActor* MonsterActor : FoundMonsters)
	{
		AMonsterSpawner* Spawner = Cast<AMonsterSpawner>(MonsterActor);
		if (Spawner)
		{
			// 스포너가 몬스터를 스폰할 때마다 OnMonsterSpawned 함수를 호출하도록 바인딩합니다.
			Spawner->OnMonsterSpawned.AddDynamic(this, &APoolingManager::OnMonsterSpawned);
		}
	}
}

//몬스터가 새로운 몬스터를 스폰할때마다 이 함수가 호출됨
void APoolingManager::OnMonsterSpawned(AMonsterBase* NewMonster)
{
	if (NewMonster)
	{
		// 새로 스폰된 몬스터의 죽음 델리게이트에 우리의 함수를 바인딩합니다.
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

