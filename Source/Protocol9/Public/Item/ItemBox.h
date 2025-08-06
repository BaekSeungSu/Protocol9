#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Item/ObjectPoolingComponent.h"
#include "ItemBox.generated.h"

UCLASS()
class PROTOCOL9_API AItemBox : public AActor
{
	GENERATED_BODY()
	
public:	
	AItemBox();

protected:
	
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Box")
	UStaticMeshComponent* StaticMesh;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Box")
	UBoxComponent* BoxComponent;
	UFUNCTION()
	virtual void OnItemOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
	UPROPERTY()
	UObjectPoolingComponent* OwningPool;
	
	virtual void ActivateItem(AActor* Activator);

};
