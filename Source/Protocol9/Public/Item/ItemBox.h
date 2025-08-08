#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item/ObjectPoolingComponent.h"
#include "ItemBox.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBoxDestroyedLocation, FVector, Location);


UCLASS()
class PROTOCOL9_API AItemBox : public AActor
{
	GENERATED_BODY()
	
public:	
	AItemBox();

	UPROPERTY(BlueprintAssignable)
	FOnBoxDestroyedLocation OnBoxDestroyedLocation;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Box")
	UStaticMeshComponent* StaticMesh;
	
	
	UPROPERTY()
	UObjectPoolingComponent* OwningPool;

	UPROPERTY(EditAnywhere, Category = "Box HP")
	float MaxHP = 30.0f;
    
	float CurrentHP;
    
	
	void OnDestroyed();

};
