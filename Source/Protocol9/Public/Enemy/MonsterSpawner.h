#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemy/MonsterSpawnRow.h"
#include "MonsterSpawner.generated.h"
class AMainCharacter;
UCLASS()
class PROTOCOL9_API AMonsterSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AMonsterSpawner();

	UFUNCTION()
	void ReSpawnMonster(AMonsterBase* Monster);
	UFUNCTION()
	void SpawnBossMonster();
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	UFUNCTION()
	void SpawnAllMonsters();
	UFUNCTION()
	void SpawnMonstersForLevel(int32 Level);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	UDataTable* MonsterSpawnTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	float SpawnRadius = 3500.0f;

	FVector GetRandomNavLocation();

	void SpawnMonster(TSubclassOf<AMonsterBase> MonsterClass);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawner")
	AMainCharacter* Player;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawner")
	int32 CurrentSpawnLevel =5;
private:
	TMap<TSubclassOf<AMonsterBase>, int32> SpawnedCount;
	
	

};
