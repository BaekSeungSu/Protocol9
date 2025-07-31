#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileBase.generated.h"

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

public:
	// 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USceneComponent* SceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* StaticMeshComponent;

	// 인스턴스 메시 리스트
	UPROPERTY(EditAnywhere, Category = "Instance")
	TArray<UStaticMesh*> InstanceMeshList;

	// 생성된 인스턴스 컴포넌트 추적
	UPROPERTY()
	TArray<class UInstancedStaticMeshComponent*> SpawnedInstance;

	// 스폰 수량
	UPROPERTY(EditAnywhere, Category = "Instance")
	int32 SpawnQuantity = 10;

	// 중첩 방지 반경
	UPROPERTY(EditAnywhere, Category = "Instance")
	float SpawnRadius = 50.f;
};
