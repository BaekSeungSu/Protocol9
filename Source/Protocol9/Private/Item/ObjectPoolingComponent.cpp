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
	
    InitializePool();			//게임 시작시 폴을 초기화 하는 함수 호출
}


void UObjectPoolingComponent::InitializePool()			// 폴을 초기화 하는 함수 아이템을 미리 생성
{
    // 데이터 테이블이 설정되지 않았다면 풀 초기화 함수를 종료
    if (!ItemSpawnTable)
    {
       return;
    }

    // 데이터 테이블의 모든 행을 가져와 TArray에 저장
    TArray<FSpawnItem*> AllItems;
    ItemSpawnTable->GetAllRows<FSpawnItem>(TEXT(""), AllItems);

    if (AllItems.IsEmpty())
    {
       UE_LOG(LogTemp, Warning, TEXT("ItemSpawnTable is empty"));
       return;
    }

    UWorld* World = GetWorld();
    if (!World) return;

    // 데이터 테이블의 각 아이템 정보에 대해 반복
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

	// 데이터 테이블의 모든 행을 가져와 TArray에 저장
    TArray<FSpawnItem*> AllItems;
	static const FString ContextString = TEXT("ItemSpawnContext");
    ItemSpawnTable->GetAllRows<FSpawnItem>(ContextString, AllItems);

    if (AllItems.IsEmpty()) return nullptr;

	//가중치 계산 
    float TotalChance = 0.0f;
    for (const FSpawnItem* SpawnData : AllItems)
    {
    	if (SpawnData)
    	{
    		TotalChance += SpawnData->SpawnChance;
    	}
    }

    // 0에서 총합 사이의 랜덤 값을 얻음
    const float RandomWeight = FMath::FRandRange(0.0f, TotalChance);
	float AccumulatedChance = 0.0f;

    // 랜덤 값에 해당하는 아이템을 찾음
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
    // 데이터 테이블에서 GetRandomItemClassFromTable() 함수를 호출하여 확률에 따라 무작위 클래스 선택
    TSubclassOf<AItemBase> ItemClassToSpawn = GetRandomItemClassFromTable();
    
    if (ItemClassToSpawn)
    {
       // 풀에서 해당 클래스를 가진 비활상화된 아이템을 찾아 가져옴
       AItemBase* ItemToSpawn = GetPooledObjectByClass(ItemClassToSpawn); 
       if (ItemToSpawn)
       {
           ItemToSpawn->SetActorLocation(Location); // 아이템을 지정된 위치에 스폰
           ItemToSpawn->SetActorRotation(FRotator::ZeroRotator);
       	   ItemToSpawn->ItemLifeTime();	
           return ItemToSpawn;
       }
    }
    return nullptr;
}

// 풀에서 특정 클래스의 아이템을 찾아 반환하는 함수
AItemBase* UObjectPoolingComponent::GetPooledObjectByClass(TSubclassOf<AItemBase> ItemClass)
{
	// 풀(ObjectPool)을 순회하며 비활성화된 아이템을 찾음
    for (AItemBase* PooledItem : ObjectPool)
    {
    	// 아이템이 유효하고, 숨겨져 있으며, 원하는 클래스와 일치하는지 확인
        if (PooledItem && PooledItem->IsHidden() && PooledItem->GetClass() == ItemClass)
        {
        	// 아이템을 활성화 상태로 변경
            PooledItem->SetActorHiddenInGame(false);
            PooledItem->SetActorEnableCollision(true);
        	PooledItem->OwningPool = this;
        	UE_LOG(LogTemp, Log, TEXT("Pool :Item '%s' spawned."), *PooledItem->GetClass()->GetName());
            return PooledItem;
        }
    }
	UE_LOG(LogTemp, Warning, TEXT("'%s' Pool is Empty!"), *ItemClass->GetName());
    return nullptr;
}

// 사용이 끝난 아이템을 풀로 되돌리는 함수
void UObjectPoolingComponent::ReturnObjectToPool(AItemBase* ObjectToReturn)
{
    if (ObjectToReturn)
    {
        ObjectToReturn->SetActorHiddenInGame(true);
        ObjectToReturn->SetActorEnableCollision(false);
    	ObjectToReturn->OwningPool = nullptr;
    	UE_LOG(LogTemp, Log, TEXT("'%s' Return to pool."), *ObjectToReturn->GetClass()->GetName());
    }
}