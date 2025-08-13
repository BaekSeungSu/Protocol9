// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/MonsterSpawner.h"
#include "Enemy/MonsterBase.h"
#include "Enemy/BossMonsterBase.h"
#include "NavigationSystem.h"
#include "Character/MainCharacter.h"
#include "Kismet/GameplayStatics.h"
// Sets default values
AMonsterSpawner::AMonsterSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void AMonsterSpawner::SpawnLevelUp(int32 PlayerLevel)
{
	if (CurrentSpawnLevel < PlayerLevel)
	{
		if (PlayerLevel > 0 && PlayerLevel < MAX_LEVEL)
		{
			CurrentSpawnLevel = PlayerLevel;
			SpawnMonstersForLevel(CurrentSpawnLevel);
		}
		else if (PlayerLevel == MAX_LEVEL)
		{
			CurrentSpawnLevel = PlayerLevel;
			Player->LevelUPEvent.RemoveDynamic(this, &AMonsterSpawner::SpawnLevelUp);
			SpawnBossMonster();
		}
	}
}

void AMonsterSpawner::BeginPlay()
{
	Super::BeginPlay();
	Player = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) ;
	if (!Player)
	CurrentSpawnLevel = Player->GetCharacterLevel();
	if (CurrentSpawnLevel == 0)
		return;
	SpawnMonstersForLevel(CurrentSpawnLevel);
	Player->LevelUPEvent.AddDynamic(this, &AMonsterSpawner::SpawnLevelUp);
}

void AMonsterSpawner::SpawnAllMonsters()
{
	if (!MonsterSpawnTable)
		return;
	TArray<FMonsterSpawnRow*> MonsterSpawnRows;
	MonsterSpawnTable->GetAllRows(TEXT("MonsterSpawnTableRead"), MonsterSpawnRows);

	for (const auto* Data : MonsterSpawnRows)
	{
		if (Data && Data->MonsterClass && Data->MonsterSpawnCountPerCharacterLevel[0] > 0)
		{
			for (int i=0 ; i< Data->MonsterSpawnCountPerCharacterLevel[0] ; i++)
			{
				SpawnMonster(Data->MonsterClass);
			}
		}
	}
	
}

void AMonsterSpawner::SpawnMonstersForLevel(int32 Level)
{
	if (!MonsterSpawnTable)
		return;
	TArray<FMonsterSpawnRow*> MonsterSpawnRows;
	
	MonsterSpawnTable->GetAllRows(TEXT("MonsterSpawnTableRead"), MonsterSpawnRows);
	for (const auto* Data : MonsterSpawnRows)
	{
		if (!Data || !Data->MonsterClass)
			return;
		const int32 TargetCount = Data->MonsterSpawnCountPerCharacterLevel[Level - 1];
		const int32 CurrentCount = SpawnedCount.FindRef(Data->MonsterClass);
		if (TargetCount > CurrentCount)
		{
			for (int i = CurrentCount; i < TargetCount; i++)
			{
				SpawnMonster(Data->MonsterClass);
			}
		}
		SpawnedCount.FindOrAdd(Data->MonsterClass) = TargetCount;
	}
}

void AMonsterSpawner::ReSpawnMonster(AMonsterBase* Monster)
{
	SpawnMonster(Monster->GetClass());
}

void AMonsterSpawner::SpawnBossMonster()
{
	DeleteAllMonsters();
	if (!BossMonsterClass)
		return;
	FVector SpawnLocation = GetRandomNavLocation();
	SpawnLocation.Z = BossMonsterClass->GetDefaultObject<ABossMonsterBase>()->GetMonsterHalfHeight();
	FRotator SpawnRotation = FRotator::ZeroRotator;
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ABossMonsterBase* NewBoss = GetWorld()->SpawnActor<ABossMonsterBase>(
		BossMonsterClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);
	OnBossMonsterSpawned.Broadcast(NewBoss); //보스 몬스터 스폰 이벤트 발생
}


FVector AMonsterSpawner::GetRandomNavLocation()
{
	if (!Player)
		return FVector::ZeroVector;
	FVector Origin = Player->GetActorLocation();
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	FNavLocation NavLocation;
	if (NavSys)
	{
		for (int i=0 ; i< MaxSpawnAttemps ; i++) // 플레이어 바로 앞에서 몬스터가 스폰되면 불쾌해서 만약 최소 거리보다 가까운 위치에 스폰될 것 같으면 최대 10번까지는 다시 새로운 위치를 찾게 했습니다.
		{
			if (NavSys->GetRandomReachablePointInRadius(Origin, MaxSpawnRadius, NavLocation))
			{
				if (FVector::Dist(Origin, NavLocation.Location) >= MinSpawnRadius)
				{
					break;
				}
					
			}
		}
		return NavLocation.Location;
	}
	return Origin + (Player->GetActorForwardVector() * MinSpawnRadius);
}

void AMonsterSpawner::SpawnMonster(TSubclassOf<AMonsterBase> MonsterClass)
{
	if (!MonsterClass)
		return;
	FVector SpawnLocation = GetRandomNavLocation();
	SpawnLocation.Z = MonsterClass->GetDefaultObject<AMonsterBase>()->GetMonsterHalfHeight();
	FRotator SpawnRotation = FRotator::ZeroRotator;
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AMonsterBase* NewMonster = GetWorld()->SpawnActor<AMonsterBase>(
		MonsterClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);
	if (!NewMonster)
		return;
	SpawnedMonsters.Add(NewMonster);
	NewMonster->OnMonsterDead.AddDynamic(this, &AMonsterSpawner::ReSpawnMonster);
	NewMonster->OnMonsterDead.AddDynamic(this, &AMonsterSpawner::ClearSpawnedMonster);
	OnMonsterSpawned.Broadcast(NewMonster);
}

void AMonsterSpawner::ClearSpawnedMonster(AMonsterBase* Monster)
{
	SpawnedMonsters.Remove(Monster);
}

void AMonsterSpawner::DeleteAllMonsters()
{
	for (AMonsterBase* Monster : SpawnedMonsters)
	{
		Monster->DeleteMonster();
	}
}


