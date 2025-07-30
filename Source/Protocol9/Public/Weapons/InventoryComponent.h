#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class AWeaponBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROTOCOL9_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();
	void AddWeapon(TSubclassOf<AWeaponBase> WeaponClass);
	AWeaponBase* GetCurrentWeapon();
	void SwapToNextWeapon();
	void SwapWeaponSlots();
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleInstanceOnly, Category="Inventory")
	TArray<AWeaponBase*> WeaponSlots;

	UPROPERTY(VisibleInstanceOnly, Category="Inventory")
	int32 CurrentWeaponIndex;
	
private:
	
};
