#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemy/MonsterBase.h"
#include "Item/ItemBox.h"
#include "Enemy/MonsterSpawner.h"
#include "PoolingManager.generated.h"

class UObjectPoolingComponent;

UCLASS()
class PROTOCOL9_API APoolingManager : public AActor
{
	GENERATED_BODY()
	
public:	
	
	APoolingManager();

protected:
	
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UObjectPoolingComponent* ObjectPoolingComponent;

	UFUNCTION()
	void MonsterDeathLocation(FVector Location);
	UFUNCTION()
	void OnMonsterSpawned(AMonsterBase* NewMonster);

	UFUNCTION()
	void BoxDestroyedLocation(FVector Location);

};
