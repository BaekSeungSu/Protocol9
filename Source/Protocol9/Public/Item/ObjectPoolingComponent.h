#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/ItemBase.h"
#include "Engine/DataTable.h"
#include "SpawnItem.h"
#include "ObjectPoolingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROTOCOL9_API UObjectPoolingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UObjectPoolingComponent();

	// 아이템 정보가 담긴 데이터 테이블
	UPROPERTY(EditAnywhere, Category = "Object Pooling")
	UDataTable* ItemSpawnTable;                  
	
	// 풀에서 특정 클래스의 아이템을 가져오는 함수
	AItemBase* GetPooledObjectByClass(TSubclassOf<AItemBase> ItemClass);

	// 사용이 끝난 아이템을 풀로 되돌리는 함수
	void ReturnObjectToPool(AItemBase* ObjectToReturn);

	// 데이터 테이블의 확률에 따라 랜덤 아이템을 풀에서 가져와서 스폰하는 함수
	AItemBase* SpawnRandomItem(const FVector& Location);

protected:
	virtual void BeginPlay() override;

private:
	// 게임 시작 시 풀을 초기화하는 함수
	void InitializePool();

	// 데이터 테이블에서 확률에 따라 랜덤 아이템 클래스를 선택하는 함수
	TSubclassOf<AItemBase> GetRandomItemClassFromTable();
	// 미리 만들어둔 아이템들을 담는 배열
	TArray<AItemBase*> ObjectPool;
};