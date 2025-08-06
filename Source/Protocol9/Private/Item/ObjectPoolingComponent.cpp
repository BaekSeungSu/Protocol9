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
    // ������ ���̺��� �������� �ʾҴٸ� Ǯ �ʱ�ȭ �Լ��� �����մϴ�.
    if (!ItemSpawnTable)
    {
       UE_LOG(LogTemp, Error, TEXT("ItemSpawnTable is not set on ObjectPoolingComponent!"));
       return;
    }

    // ������ ���̺��� ��� ���� ������ TArray�� �����մϴ�.
    TArray<FSpawnItem*> AllItems;
    ItemSpawnTable->GetAllRows<FSpawnItem>(TEXT(""), AllItems);

    if (AllItems.Num() == 0)
    {
       UE_LOG(LogTemp, Warning, TEXT("ItemSpawnTable is empty. No items to pool."));
       return;
    }

    UWorld* World = GetWorld();
    if (!World) return;

    // ������ ���̺��� �� ������ ������ ���� �ݺ��մϴ�.
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

    // 0���� ���� ������ ���� ���� ����ϴ�.
    float RandomWeight = FMath::FRandRange(0.0f, TotalWeight);

    // ���� ���� �ش��ϴ� �������� ã���ϴ�.
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
    // ������ ���̺��� ���� ������ Ŭ���� ����
    TSubclassOf<AItemBase> ItemClassToSpawn = GetRandomItemClassFromTable();
    
    if (ItemClassToSpawn)
    {
       // Ǯ���� �ش� Ŭ������ �������� ã�� �����ɴϴ�.
       AItemBase* ItemToSpawn = GetPooledObjectByClass(ItemClassToSpawn); 
       if (ItemToSpawn)
       {
           ItemToSpawn->SetActorLocation(Location); // �������� ������ ��ġ�� ����
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