// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/TileManager.h"

#include "EngineUtils.h"
#include "NavigationSystem.h"
#include "DSP/AudioDebuggingUtilities.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Map/Tile/TileBase.h"
#include "NavMesh/NavMeshBoundsVolume.h"

// Sets default values
ATileManager::ATileManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATileManager::BeginPlay()
{
	Super::BeginPlay();

	Character = Cast<ACharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	
	TArray<AActor*> FoundTiles;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),ATileBase::StaticClass(),FoundTiles);

	TileArray.Init(nullptr, 9);

	for (AActor* Actor : FoundTiles)
	{
		ATileBase* Tile = Cast<ATileBase>(Actor);
		if (Tile&&Tile->TileIndex>=0&&Tile->TileIndex<=8)
		{
			TileArray[Tile->TileIndex] = Tile;
		}
	}

	CurrentPlayerTileCoord = GetPlayerTileCoord();

	if (!NavMeshBoundsVolume)
	{
		for (TActorIterator<ANavMeshBoundsVolume>It(GetWorld()); It; ++It)
		{
			NavMeshBoundsVolume = *It;
			break;
		}
	}
	
}

// Called every frame
void ATileManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateTilesIfNeeded();
}

int32 ATileManager::GetPlayerTileCoord() const
{
	if (!Character)
	{
		UE_LOG(LogTemp,Warning,TEXT("NO CHARACTER"));
		return -1;
	}

	if (!TileArray.IsValidIndex(4) || !TileArray[4])
	{
		UE_LOG(LogTemp,Warning,TEXT("TILE4 INVALID"));
		return -1;
	}

	const FVector CenterTileLocation = TileArray[4] ->GetActorLocation();
	const FVector PlayerLocation = Character->GetActorLocation();

	//플레이어 위치를 중앙 타일 기준으로 상대 계산 - > 플레이어가 어느 타일에 있는지 계산
	float RelativeX = PlayerLocation.X - CenterTileLocation.X;
	float RelativeY = PlayerLocation.Y - CenterTileLocation.Y;

	int32 TileX = FMath::RoundToInt(RelativeX / TileSize.X);
	int32 TileY = FMath::RoundToInt(RelativeY / TileSize.Y);

	if (TileX < -1 || TileX > 1 || TileY < -1 || TileY > 1)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Character out of tile range! Coord: %d, %d"), TileX, TileY);
		return -1;
	}
	
	int32 Index = (1 - TileX) * 3 + (TileY + 1); // 0~8로 변환
	//UE_LOG(LogTemp, Warning, TEXT("Character is on tile index: %d"), Index);

	return Index;
}

void ATileManager::UpdateTilesIfNeeded()
{
	int32 NewCenterIndex = GetPlayerTileCoord();

	if (NewCenterIndex == -1 || !TileArray.IsValidIndex(NewCenterIndex)
		|| !IsValid(TileArray[NewCenterIndex])|| NewCenterIndex==4)
		return;

	//전방 이동
	// if (NewCenterIndex == 1)
	// {
	// 	for (int i: {6, 7, 8})
	// 	{
	// 		TileArray[i]->SetActorRotation(FRotator(0,FMath::RandRange(0,3)*90,0));
	// 		TileArray[i]->SetActorLocation(TileArray[i]->GetActorLocation()+FVector(TileSize.X*3,0,0));
	// 	}
	// 	ShiftTileArray(ETileShiftDirection::Forward);
	// }
	// //후방 이동
	// else if (NewCenterIndex == 7)
	// {
	// 	for (int i : {0, 1, 2})
	// 	{
	// 		TileArray[i]->SetActorRotation(FRotator(0,FMath::RandRange(0,3)*90,0));
	// 		TileArray[i]->SetActorLocation(TileArray[i]->GetActorLocation() - FVector(TileSize.X * 3, 0, 0));
	// 	}
	// 	ShiftTileArray(ETileShiftDirection::Backward);
	// }
	// // 좌측 이동
	// else if (NewCenterIndex == 3)
	// {
	// 	for (int i : {2, 5, 8})
	// 	{
	// 		TileArray[i]->SetActorRotation(FRotator(0,FMath::RandRange(0,3)*90,0));
	// 		TileArray[i]->SetActorLocation(TileArray[i]->GetActorLocation()-FVector(0, TileSize.Y * 3, 0));
	// 		UE_LOG(LogTemp, Warning, TEXT("LEFT MOVED"));
	// 	}
	// 	ShiftTileArray(ETileShiftDirection::Left);
	// }
	// //우측 이동
	// else if (NewCenterIndex == 5)
	// {
	// 	for (int i : {0,3,6})
	// 	{
	// 		TileArray[i]->SetActorRotation(FRotator(0,FMath::RandRange(0,3)*90,0));
	// 		TileArray[i]->SetActorLocation(TileArray[i]->GetActorLocation() + FVector(0, TileSize.Y*3, 0));
	// 	}
	// 	ShiftTileArray(ETileShiftDirection::Right);
	// }

	if (NewCenterIndex == 1)
	{
		TArray<ATileBase*> Moved = { TileArray[6], TileArray[7], TileArray[8] };

		for (ATileBase* T : Moved)
		{
			if (!IsValid(T)) continue;
			T->SetActorRotation(FRotator(0, FMath::RandRange(0,3)*90, 0));
			T->SetActorLocation(T->GetActorLocation() + FVector(TileSize.X*3, 0, 0));
		}

		// 위치/회전 적용 완료 → 여기서 재생성
		for (ATileBase* T : Moved)
			if (IsValid(T)) T->RebuildInstances();

		ShiftTileArray(ETileShiftDirection::Forward);
	}

	// 후방 이동 (-X)
	else if (NewCenterIndex == 7)
	{
		TArray<ATileBase*> Moved = { TileArray[0], TileArray[1], TileArray[2] };

		for (ATileBase* T : Moved)
		{
			if (!IsValid(T)) continue;
			T->SetActorRotation(FRotator(0, FMath::RandRange(0,3)*90, 0));
			T->SetActorLocation(T->GetActorLocation() - FVector(TileSize.X*3, 0, 0));
		}

		for (ATileBase* T : Moved)
			if (IsValid(T)) T->RebuildInstances();

		ShiftTileArray(ETileShiftDirection::Backward);
	}

	// 좌측 이동 (-Y)
	else if (NewCenterIndex == 3)
	{
		TArray<ATileBase*> Moved = { TileArray[2], TileArray[5], TileArray[8] };

		for (ATileBase* T : Moved)
		{
			if (!IsValid(T)) continue;
			T->SetActorRotation(FRotator(0, FMath::RandRange(0,3)*90, 0));
			T->SetActorLocation(T->GetActorLocation() - FVector(0, TileSize.Y*3, 0));
		}

		for (ATileBase* T : Moved)
			if (IsValid(T)) T->RebuildInstances();

		ShiftTileArray(ETileShiftDirection::Left);
	}

	// 우측 이동 (+Y)
	else if (NewCenterIndex == 5)
	{
		TArray<ATileBase*> Moved = { TileArray[0], TileArray[3], TileArray[6] };

		for (ATileBase* T : Moved)
		{
			if (!IsValid(T)) continue;
			T->SetActorRotation(FRotator(0, FMath::RandRange(0,3)*90, 0));
			T->SetActorLocation(T->GetActorLocation() + FVector(0, TileSize.Y*3, 0));
		}

		for (ATileBase* T : Moved)
			if (IsValid(T)) T->RebuildInstances();

		ShiftTileArray(ETileShiftDirection::Right);
	}
	if (NavMeshBoundsVolume && TileArray.IsValidIndex(4))
	{
		FVector CenterTileLocation = TileArray[4]->GetActorLocation();
		NavMeshBoundsVolume->SetActorLocation(CenterTileLocation);

		UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
		if (NavSys)
		{
			NavSys->OnNavigationBoundsUpdated(NavMeshBoundsVolume);
		}
	}
}

void ATileManager::ShiftTileArray(ETileShiftDirection Direction)
{
	TArray<ATileBase*> Old = TileArray;
	TileArray.SetNum(9);

	switch (Direction)
	{
	case ETileShiftDirection::Forward: // +X
		// 위 행(6~8)을 아래로, 나머지 한 줄씩 위로
		TileArray[0] = Old[6];
		TileArray[1] = Old[7];
		TileArray[2] = Old[8];
		TileArray[3] = Old[0];
		TileArray[4] = Old[1];
		TileArray[5] = Old[2];
		TileArray[6] = Old[3];
		TileArray[7] = Old[4];
		TileArray[8] = Old[5];
		break;

	case ETileShiftDirection::Backward: // -X
		// 아래 행(0~2)을 위로, 나머지 한 줄씩 아래로
		TileArray[0] = Old[3];
		TileArray[1] = Old[4];
		TileArray[2] = Old[5];
		TileArray[3] = Old[6];
		TileArray[4] = Old[7];
		TileArray[5] = Old[8];
		TileArray[6] = Old[0];
		TileArray[7] = Old[1];
		TileArray[8] = Old[2];
		break;

	case ETileShiftDirection::Left:
		TileArray[0] = Old[2];
		TileArray[3] = Old[5];
		TileArray[6] = Old[8];

		TileArray[1] = Old[0];
		TileArray[4] = Old[3];
		TileArray[7] = Old[6];

		TileArray[2] = Old[1];
		TileArray[5] = Old[4];
		TileArray[8] = Old[7];
		break;

	case ETileShiftDirection::Right:
		TileArray[0] = Old[1];
		TileArray[3] = Old[4];
		TileArray[6] = Old[7];

		TileArray[1] = Old[2];
		TileArray[4] = Old[5];
		TileArray[7] = Old[8];

		TileArray[2] = Old[0];
		TileArray[5] = Old[3];	
		TileArray[8] = Old[6];
		break;
	default:
		break;
	}
}


