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

	// ������ ������ ��� ������ ���̺�
	UPROPERTY(EditAnywhere, Category = "Object Pooling")
	UDataTable* ItemSpawnTable;                  
	
	// Ǯ���� Ư�� Ŭ������ �������� �������� �Լ�
	AItemBase* GetPooledObjectByClass(TSubclassOf<AItemBase> ItemClass);

	// ����� ���� �������� Ǯ�� �ǵ����� �Լ�
	void ReturnObjectToPool(AItemBase* ObjectToReturn);

	// ������ ���̺��� Ȯ���� ���� ���� �������� Ǯ���� �����ͼ� �����ϴ� �Լ�
	AItemBase* SpawnRandomItem(const FVector& Location);

protected:
	virtual void BeginPlay() override;

private:
	// ���� ���� �� Ǯ�� �ʱ�ȭ�ϴ� �Լ�
	void InitializePool();

	// ������ ���̺��� Ȯ���� ���� ���� ������ Ŭ������ �����ϴ� �Լ�
	TSubclassOf<AItemBase> GetRandomItemClassFromTable();
	// �̸� ������ �����۵��� ��� �迭
	TArray<AItemBase*> ObjectPool;
};