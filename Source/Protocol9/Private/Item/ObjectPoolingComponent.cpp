#include "Item/ObjectPoolingComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Item/ItemBase.h"

UObjectPoolingComponent::UObjectPoolingComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UObjectPoolingComponent::BeginPlay()
{
    Super::BeginPlay();
    InitializePool();
}

void UObjectPoolingComponent::InitializePool()
{
    // 데이터 테이블이 설정되지 않았다면 풀 초기화 함수를 종료합니다.
    if (!ItemSpawnTable)
    {
       UE_LOG(LogTemp, Error, TEXT("ItemSpawnTable is not set on ObjectPoolingComponent!"));
       return;
    }

    // 데이터 테이블의 모든 행을 가져와 TArray에 저장합니다.
    TArray<FSpawnItem*> AllItems;
    ItemSpawnTable->GetAllRows<FSpawnItem>(TEXT(""), AllItems);

    if (AllItems.Num() == 0)
    {
       UE_LOG(LogTemp, Warning, TEXT("ItemSpawnTable is empty. No items to pool."));
       return;
    }

    UWorld* World = GetWorld();
    if (!World) return;

    // 데이터 테이블의 각 아이템 정보에 대해 반복합니다.
    for (const FSpawnItem* SpawnData : AllItems)
    {
       if (SpawnData && SpawnData->SpawnItem)
       {
          
          const int32 PoolSize = 5; 
          for (int32 i = 0; i < PoolSize; ++i)
          {
             AItemBase* NewItem = World->SpawnActor<AItemBase>(SpawnData->SpawnItem);
             if (NewItem)
             {
                NewItem->SetActorHiddenInGame(true);
                NewItem->SetActorEnableCollision(false);
                ObjectPool.Add(NewItem);
             }
          }
       }
    }
}

TSubclassOf<AItemBase> UObjectPoolingComponent::GetRandomItemClassFromTable()
{
    if (!ItemSpawnTable) return nullptr;

    TArray<FSpawnItem*> AllItems;
    ItemSpawnTable->GetAllRows<FSpawnItem>(TEXT(""), AllItems);

    if (AllItems.Num() == 0) return nullptr;
	
    float TotalWeight = 0.0f;
    for (FSpawnItem* SpawnData : AllItems)
    {
       TotalWeight += SpawnData->SpawnChance;
    }

    // 0에서 총합 사이의 랜덤 값을 얻습니다.
    float RandomWeight = FMath::FRandRange(0.0f, TotalWeight);

    // 랜덤 값에 해당하는 아이템을 찾습니다.
    for (FSpawnItem* SpawnData : AllItems)
    {
       if (RandomWeight <= SpawnData->SpawnChance)
       {
           return SpawnData->SpawnItem;
       }
       RandomWeight -= SpawnData->SpawnChance;
    }
    return nullptr;
}

AItemBase* UObjectPoolingComponent::SpawnRandomItem(const FVector& Location)
{
    // 데이터 테이블에서 랜덤 아이템 클래스 선택
    TSubclassOf<AItemBase> ItemClassToSpawn = GetRandomItemClassFromTable();
    
    if (ItemClassToSpawn)
    {
       // 풀에서 해당 클래스의 아이템을 찾아 가져옵니다.
       AItemBase* ItemToSpawn = GetPooledObjectByClass(ItemClassToSpawn); 
       if (ItemToSpawn)
       {
           ItemToSpawn->SetActorLocation(Location); // 아이템을 지정된 위치에 스폰
           ItemToSpawn->SetActorRotation(FRotator::ZeroRotator);
           return ItemToSpawn;
       }
    }
    return nullptr;
}

AItemBase* UObjectPoolingComponent::GetPooledObjectByClass(TSubclassOf<AItemBase> ItemClass)
{
    for (AItemBase* PooledItem : ObjectPool)
    {
        if (PooledItem && PooledItem->IsHidden() && PooledItem->GetClass() == ItemClass)
        {
            PooledItem->SetActorHiddenInGame(false);
            PooledItem->SetActorEnableCollision(true);
        	PooledItem->OwningPool = this;
            
            return PooledItem;
        }
    }
    return nullptr;
}

void UObjectPoolingComponent::ReturnObjectToPool(AItemBase* ObjectToReturn)
{
    if (ObjectToReturn)
    {
        ObjectToReturn->SetActorHiddenInGame(true);
        ObjectToReturn->SetActorEnableCollision(false);
    	ObjectToReturn->OwningPool = nullptr;
    }
}