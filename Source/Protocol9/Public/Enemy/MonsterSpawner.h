#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemy/MonsterSpawnRow.h"
#include "MonsterSpawner.generated.h"
class AMainCharacter;
class ABossMonsterBase;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMonsterSpawned, AMonsterBase*, Monster);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBossMonsterSpawned, ABossMonsterBase*, BossMonster);
UCLASS(Blueprintable)
class PROTOCOL9_API AMonsterSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AMonsterSpawner();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Level")
	int32 MAX_LEVEL = 10;
	UFUNCTION()
	void ReSpawnMonster(AMonsterBase* Monster);
	UFUNCTION()
	void SpawnBossMonster();
	UPROPERTY(BlueprintAssignable)
	FMonsterSpawned OnMonsterSpawned;
	UPROPERTY(BlueprintAssignable)
	FBossMonsterSpawned OnBossMonsterSpawned;
	UFUNCTION()
	void SpawnLevelUp(int32 PlayerLevel);
protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	void SpawnAllMonsters();
	UFUNCTION()
	void SpawnMonstersForLevel(int32 Level);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	UDataTable* MonsterSpawnTable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	TSubclassOf<ABossMonsterBase> BossMonsterClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnRadius")
	float MaxSpawnRadius = 3500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnRadius")
	float MinSpawnRadius = 500.0f;

	int32 MaxSpawnAttemps =10;
	FVector GetRandomNavLocation();

	void SpawnMonster(TSubclassOf<AMonsterBase> MonsterClass);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawner")
	AMainCharacter* Player;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawner")
	int32 CurrentSpawnLevel =1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpawnedMonster")
	TArray<TObjectPtr<AMonsterBase>> SpawnedMonsters;
	UFUNCTION()
	void ClearSpawnedMonster(AMonsterBase* Monster);
	UFUNCTION()
	void DeleteAllMonsters();
private:
	TMap<TSubclassOf<AMonsterBase>, int32> SpawnedCount;
};
