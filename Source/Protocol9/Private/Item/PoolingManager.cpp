#include "Item/PoolingManager.h"
#include "Enemy/MonsterBase.h"
#include "Kismet/GameplayStatics.h"
#include "Item/ItemBox.h"
#include "Enemy/MonsterSpawner.h"
#include "Item/ObjectPoolingComponent.h"


APoolingManager::APoolingManager()
{
	PrimaryActorTick.bCanEverTick = false;

	// ������Ʈ Ǯ�� ������Ʈ�� �����Ͽ� �� �Ŵ����� �����մϴ�.
	ObjectPoolingComponent = CreateDefaultSubobject<UObjectPoolingComponent>(TEXT("ObjectPoolingComponent"));
	
}


void APoolingManager::BeginPlay()
{
	Super::BeginPlay();


	//���忡 �ִ� ��� AMonsterSpawner ���͸� ã��
	TArray<AActor*> FoundMonsters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMonsterSpawner::StaticClass(), FoundMonsters);

	//ã�� �����ʸ��� ��������Ʈ ����
	for (AActor* MonsterActor : FoundMonsters)
	{
		AMonsterSpawner* Spawner = Cast<AMonsterSpawner>(MonsterActor);
		if (Spawner)
		{
			// �����ʰ� ���͸� ������ ������ OnMonsterSpawned �Լ��� ȣ���ϵ��� ���ε��մϴ�.
			Spawner->OnMonsterSpawned.AddDynamic(this, &APoolingManager::OnMonsterSpawned);
		}
	}
	// ���忡 �ִ� ��� AItemBox ���͸� ã���ϴ�.
	TArray<AActor*> FoundeBox;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AItemBox::StaticClass(), FoundeBox);
	for (AActor* FoundeBoxActor : FoundeBox)
	{
		AItemBox* Box = Cast<AItemBox>(FoundeBoxActor);
		if (Box)
		{
			Box->OnBoxDestroyedLocation.AddDynamic(this,&APoolingManager::BoxDestroyedLocation);
		}
	}
}

//���Ͱ� ���ο� ���͸� �����Ҷ����� �� �Լ��� ȣ���
void APoolingManager::OnMonsterSpawned(AMonsterBase* NewMonster)
{
	if (NewMonster)
	{
		// ���� ������ ������ ���� ��������Ʈ�� �츮�� �Լ��� ���ε�.
		NewMonster->OnMonsterDeadLocation.AddDynamic(this, &APoolingManager::MonsterDeathLocation);
	}
}

// ������ ���� ��������Ʈ�� ȣ��� �� ����Ǵ� �Լ�
void APoolingManager::MonsterDeathLocation(FVector Location)
{
	UObjectPoolingComponent* PoolingComp = FindComponentByClass<UObjectPoolingComponent>();
	if (!PoolingComp) return;
	
	int32 RandValue = FMath::RandRange(1, 100);
	if (RandValue<=30)
	{
		PoolingComp -> SpawnRandomItem(Location);
	}
}

void APoolingManager::BoxDestroyedLocation(FVector Location)
{
	UObjectPoolingComponent* PoolingComp = FindComponentByClass<UObjectPoolingComponent>();
	if (!PoolingComp) return;

	PoolingComp -> SpawnRandomItem(Location);
}

