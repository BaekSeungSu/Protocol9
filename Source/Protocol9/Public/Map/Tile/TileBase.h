#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileBase.generated.h"

USTRUCT(BlueprintType)
struct FSpawnSpec
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> Class;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0"))
	int32 Count = 0;
};

USTRUCT(BlueprintType)
struct FWeaponSpec
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> Class = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Count = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Scale = {1.0f, 1.0f,1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ZOffset = 0.0f ;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ClearRadius = 300.f; // 빈자리 검사 반경, 무기 크기 만큼은 비우기, 넉넉하게 300쯤
};

UCLASS()
class PROTOCOL9_API ATileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ATileBase();

protected:
	virtual void BeginPlay() override;

private:
	// 내부 함수
	void ClearPreviousInstances();
	void GenerateInstances();
	void SpawnWeapons();    
public:
	// 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USceneComponent* SceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* StaticMeshComponent;

	// 클래스별 스폰 스펙
	UPROPERTY(EditAnywhere, Category = "SpawnClass")
	TArray<FSpawnSpec> SpawnSpecs;

	// 생성된 인스턴스 추적
	UPROPERTY()
	TArray<AActor*> SpawnedClass;

	// 무기 스폰 리스트
	UPROPERTY(VisibleAnywhere)
	TArray<AActor*> SpawnedWeapons;

	// 생성된 무기 추적
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spawn|Weapon")
	TArray<FWeaponSpec> WeaponSpecs;

	// 중첩 방지 반경
	UPROPERTY(EditAnywhere, Category = "Instance")
	float SpawnRadius = 50.f;

	// 스폰 시도 횟수 배율
	UPROPERTY(EditAnywhere, Category = "Instance")
	int32 MaxAttemptsPerSpawn = 20;
	
	UPROPERTY(EditAnywhere, Category = "Tile info")
	int32 TileIndex;
	
	UFUNCTION(BlueprintCallable, Category="Spawn")
	void RebuildInstances();

	
#if WITH_EDITOR
	void PrintIndex();
#endif
};
