#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemInterface.h"
#include "ItemBase.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
class USphereComponent;

class UObjectPoolingComponent;

UCLASS()
class PROTOCOL9_API AItemBase : public AActor,public IItemInterface
{
	GENERATED_BODY()
	
public:	
	
	AItemBase();

	UPROPERTY()
	UObjectPoolingComponent* OwningPool;
	UFUNCTION()
	void ItemLifeTime();
	UFUNCTION()
	void ReturnToPool();

protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Item")
	FName ItemType;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Item")
	float ItemDuration;
	UPROPERTY(EditAnywhere,Category="Item")
	float ItemLifeDuration;
	
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void OnItemOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;
	UFUNCTION()
	
	virtual void ActivateItem(AActor* Activator)override;
	virtual FName GetItemType() const override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Item|Niagara")
	UNiagaraComponent* NiagaraComp;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Item|Niagara")
	UNiagaraSystem* NiagaraSystem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Niagara")
	UNiagaraSystem* OverlapEffectSystem;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Item|Component")
	USphereComponent* Collision;

	FTimerHandle EffectTimerHandle;

	FTimerHandle ItemLIfeTimerHandle;
	
	virtual void EndEffect() override;
	
	
};
