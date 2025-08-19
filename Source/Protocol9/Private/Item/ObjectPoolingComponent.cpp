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
	
    InitializePool();			//���� ���۽� ���� �ʱ�ȭ �ϴ� �Լ� ȣ��
}


void UObjectPoolingComponent::InitializePool()			// ���� �ʱ�ȭ �ϴ� �Լ� �������� �̸� ����
{
    // ������ ���̺��� �������� �ʾҴٸ� Ǯ �ʱ�ȭ �Լ��� ����
    if (!ItemSpawnTable)
    {
       return;
    }

    // ������ ���̺��� ��� ���� ������ TArray�� ����
    TArray<FSpawnItem*> AllItems;
	static const FString ContextString = TEXT("ItemSpawnContext");
    ItemSpawnTable->GetAllRows<FSpawnItem>(ContextString, AllItems);

    if (AllItems.IsEmpty())
    {
       UE_LOG(LogTemp, Warning, TEXT("ItemSpawnTable is empty"));
       return;
    }

    UWorld* World = GetWorld();
    if (!World) return;

    // ������ ���̺��� �� ������ ������ ���� �ݺ�
    for (const FSpawnItem* SpawnData : AllItems)
    {
       if (SpawnData && SpawnData->SpawnItem)
       {
          
          const int32 PoolSize = SpawnData->PoolSize;
       	  UE_LOG(LogTemp, Warning, TEXT("'%s' Item %d make."), *SpawnData->ItemName.ToString(), PoolSize);
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
	UE_LOG(LogTemp, Warning, TEXT(" POOLING Completed. PoolSize: %d "), ObjectPool.Num());
}

TSubclassOf<AItemBase> UObjectPoolingComponent::GetRandomItemClassFromTable()
{
    if (!ItemSpawnTable) return nullptr;

	// ������ ���̺��� ��� ���� ������ TArray�� ����
    TArray<FSpawnItem*> AllItems;
	static const FString ContextString = TEXT("ItemSpawnContext");
    ItemSpawnTable->GetAllRows<FSpawnItem>(ContextString, AllItems);

    if (AllItems.IsEmpty()) return nullptr;

	//����ġ ��� 
    float TotalChance = 0.0f;
    for (const FSpawnItem* SpawnData : AllItems)
    {
    	if (SpawnData)
    	{
    		TotalChance += SpawnData->SpawnChance;
    	}
    }

    // 0���� ���� ������ ���� ���� ����
    const float RandomWeight = FMath::FRandRange(0.0f, TotalChance);
	float AccumulatedChance = 0.0f;

    // ���� ���� �ش��ϴ� �������� ã��
    for (const FSpawnItem* SpawnData : AllItems)
    {
    	AccumulatedChance += SpawnData->SpawnChance;
       if (RandomWeight <= AccumulatedChance)
       {
           return SpawnData->SpawnItem;
       }
    }
    return nullptr;
}

AItemBase* UObjectPoolingComponent::SpawnRandomItem(const FVector& Location)
{
    // ������ ���̺��� GetRandomItemClassFromTable() �Լ��� ȣ���Ͽ� Ȯ���� ���� ������ Ŭ���� ����
    TSubclassOf<AItemBase> ItemClassToSpawn = GetRandomItemClassFromTable();
    
    if (ItemClassToSpawn)
    {
       // Ǯ���� �ش� Ŭ������ ���� ��Ȱ��ȭ�� �������� ã�� ������
       AItemBase* ItemToSpawn = GetPooledObjectByClass(ItemClassToSpawn); 
       if (ItemToSpawn)
       {
           ItemToSpawn->SetActorLocation(Location); // �������� ������ ��ġ�� ����
           ItemToSpawn->SetActorRotation(FRotator::ZeroRotator);
       	   ItemToSpawn->ItemLifeTime();	
           return ItemToSpawn;
       }
    }
    return nullptr;
}

// Ǯ���� Ư�� Ŭ������ �������� ã�� ��ȯ�ϴ� �Լ�
AItemBase* UObjectPoolingComponent::GetPooledObjectByClass(TSubclassOf<AItemBase> ItemClass)
{
	// Ǯ(ObjectPool)�� ��ȸ�ϸ� ��Ȱ��ȭ�� �������� ã��
    for (AItemBase* PooledItem : ObjectPool)
    {
    	if (!IsValid(PooledItem)) continue;

    	if (AActor* AsActor = Cast<AActor>(PooledItem))
    	{
    		if (AsActor->IsActorBeingDestroyed())      // �ı� ���� ���� ����
    			continue;
    	}
    	
    	// �������� ��ȿ�ϰ�, ������ ������, ���ϴ� Ŭ������ ��ġ�ϴ��� Ȯ��
    	if (PooledItem->bAvailableInPool && PooledItem->IsA(ItemClass))
    	{
    		PooledItem->OwningPool = this;
    		PooledItem->OnAcquireFromPool(); // �� ���� �ʱ�ȭ & ǥ��
    		return PooledItem;
    	}
    }
    return nullptr;
}

// ����� ���� �������� Ǯ�� �ǵ����� �Լ�
void UObjectPoolingComponent::ReturnObjectToPool(AItemBase* ObjectToReturn)
{
    if (IsValid(ObjectToReturn))
    {
    	ObjectToReturn->OnReturnToPool(); 
    	ObjectToReturn->OwningPool = nullptr;
    }
}