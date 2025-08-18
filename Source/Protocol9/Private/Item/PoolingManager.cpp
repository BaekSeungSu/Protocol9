#include "Item/PoolingManager.h"
#include "Enemy/MonsterBase.h"
#include "Kismet/GameplayStatics.h"
#include "Item/ItemBox.h"
#include "Enemy/MonsterSpawner.h"
#include "Item/ObjectPoolingComponent.h"
#include "Engine/World.h"
#include "Map/Tile/TileBase.h"


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
	TArray<AActor*> FoundBoxes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AItemBox::StaticClass(), FoundBoxes);
	for (AActor* A : FoundBoxes)
	{
		auto* Box = Cast<AItemBox>(A);
		if (!IsValid(Box)) continue;

		
		AActor* BoxOwner  = Box->GetOwner();
		if (!BoxOwner  || !BoxOwner ->IsA(ATileBase::StaticClass()))
			continue;

		if (!Box->OnBoxDestroyedLocation.IsAlreadyBound(this, &APoolingManager::BoxDestroyedLocation))
			Box->OnBoxDestroyedLocation.AddDynamic(this, &APoolingManager::BoxDestroyedLocation);
	}

	if (HasAuthority())
	{
		if (UWorld* World = GetWorld())
		{
			ActorSpawnedHandle = World->AddOnActorSpawnedHandler(
				FOnActorSpawned::FDelegate::CreateUObject(this, &APoolingManager::HandleActorSpawned)
			);
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

void APoolingManager::HandleActorSpawned(AActor* SpawnedActor)
{
	if (!HasAuthority() || !IsValid(SpawnedActor)) return;

	// 1) �ڽ� Ŭ������
	if (!SpawnedActor->IsA(AItemBox::StaticClass())) return;

	// 2) ������(owner)�� Ÿ���� ���� (���Ѹ� ���)
	AActor* BoxOwner  = SpawnedActor->GetOwner();
	if (!BoxOwner  || !BoxOwner ->IsA(ATileBase::StaticClass()))
		return;
	AItemBox* NewBox = CastChecked<AItemBox>(SpawnedActor);
	if (!NewBox->OnBoxDestroyedLocation.IsAlreadyBound(this, &APoolingManager::BoxDestroyedLocation))
	{
		NewBox->OnBoxDestroyedLocation.AddDynamic(this, &APoolingManager::BoxDestroyedLocation);
	}
}


void APoolingManager::BoxDestroyedLocation(FVector Location)
{
	if (!HasAuthority() || !ObjectPoolingComponent) return;


	ObjectPoolingComponent->SpawnRandomItem(Location);
}


