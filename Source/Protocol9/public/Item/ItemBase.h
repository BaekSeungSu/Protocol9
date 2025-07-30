#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemInterface.h"
#include "ItemBase.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
class USphereComponent;

UCLASS()
class PROTOCOL9_API AItemBase : public AActor,public IItemInterface
{
	GENERATED_BODY()
	
public:	
	
	AItemBase();

protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Item")
	FName ItemType;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Item")
	float ItemDuration;
	
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
	virtual void OnItemEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
		) override;
	
	virtual void ActivateItem(AActor* Activator)override;
	virtual FName GetItemType() const override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Item|Niagara")
	UNiagaraComponent* NiagaraComp;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Item|Niagara")
	UNiagaraSystem* NiagaraSystem;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Item|Component")
	USphereComponent* Collision;

	FTimerHandle EffectTimerHandle;
	
	virtual void EndEffect() override;

	virtual void DestroyItem();

	
};
