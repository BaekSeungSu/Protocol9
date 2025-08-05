#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemy/MonsterSpawnRow.h"
#include "MonsterSpawner.generated.h"

UCLASS()
class PROTOCOL9_API AMonsterSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AMonsterSpawner();

	UFUNCTION()
	void ReSpawnMonster(AMonsterBase* Monster);
protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	void SpawnAllMonsters();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	UDataTable* MonsterSpawnTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	float SpawnRadius = 3500.0f;

	FVector GetRandomNavLocation();

	void SpawnMonster(TSubclassOf<AMonsterBase> MonsterClass);
private:
	

	

};
