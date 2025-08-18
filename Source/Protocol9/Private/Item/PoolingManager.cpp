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

	// 오브젝트 풀링 컴포넌트를 생성하여 이 매니저에 부착합니다.
	ObjectPoolingComponent = CreateDefaultSubobject<UObjectPoolingComponent>(TEXT("ObjectPoolingComponent"));
	
}


void APoolingManager::BeginPlay()
{
	Super::BeginPlay();


	//월드에 있는 모든 AMonsterSpawner 엑터를 찾음
	TArray<AActor*> FoundMonsters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMonsterSpawner::StaticClass(), FoundMonsters);

	//찾은 스포너마다 델리게이트 연결
	for (AActor* MonsterActor : FoundMonsters)
	{
		AMonsterSpawner* Spawner = Cast<AMonsterSpawner>(MonsterActor);
		if (Spawner)
		{
			// 스포너가 몬스터를 스폰할 때마다 OnMonsterSpawned 함수를 호출하도록 바인딩합니다.
			Spawner->OnMonsterSpawned.AddDynamic(this, &APoolingManager::OnMonsterSpawned);
		}
	}
	// 월드에 있는 모든 AItemBox 액터를 찾습니다.
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

//몬스터가 새로운 몬스터를 스폰할때마다 이 함수가 호출됨
void APoolingManager::OnMonsterSpawned(AMonsterBase* NewMonster)
{
	if (NewMonster)
	{
		// 새로 스폰된 몬스터의 죽음 델리게이트에 우리의 함수를 바인딩.
		NewMonster->OnMonsterDeadLocation.AddDynamic(this, &APoolingManager::MonsterDeathLocation);
	}
}

// 몬스터의 죽음 델리게이트가 호출될 때 실행되는 함수
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

	// 1) 박스 클래스만
	if (!SpawnedActor->IsA(AItemBox::StaticClass())) return;

	// 2) 소유자(owner)가 타일일 때만 (무한맵 방어)
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


