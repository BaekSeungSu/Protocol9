// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/MonsterSpawner.h"
#include "Enemy/MonsterBase.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
// Sets default values
AMonsterSpawner::AMonsterSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void AMonsterSpawner::BeginPlay()
{
	Super::BeginPlay();
	SpawnAllMonsters();
}

void AMonsterSpawner::SpawnAllMonsters()
{
	if (!MonsterSpawnTable)
		return;
	TArray<FMonsterSpawnRow*> MonsterSpawnRows;
	MonsterSpawnTable->GetAllRows(TEXT("MonsterSpawnTableRead"), MonsterSpawnRows);

	for (const auto* Data : MonsterSpawnRows)
	{
		if (Data && Data->MonsterClass && Data->MonsterMaxCount > 0)
		{
			for (int i=0 ; i< Data->MonsterMaxCount ; i++)
			{
				SpawnMonster(Data->MonsterClass);
			}
		}
	}
	
}

void AMonsterSpawner::ReSpawnMonster(AMonsterBase* Monster)
{
	SpawnMonster(Monster->GetClass());
}




FVector AMonsterSpawner::GetRandomNavLocation()
{
	APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!Player)
		return FVector::ZeroVector;
	FVector Origin = Player->GetActorLocation();
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	FNavLocation NavLocation;
	if (NavSys && NavSys->GetRandomReachablePointInRadius(Origin, SpawnRadius, NavLocation))
	{
		return NavLocation.Location;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NavSys->GetRandomReachablePointInRadius() failed"));
		return Origin + FMath::VRand() * 5000.0f;
	}
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
	NewMonster->OnMonsterDead.AddDynamic(this, &AMonsterSpawner::ReSpawnMonster);
}


