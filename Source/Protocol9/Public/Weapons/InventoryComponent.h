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
	AWeaponBase* GetCurrentWeapon() const;

	void EquipWeaponAtIndex(int32 SlotIndex);
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleInstanceOnly, Category="Inventory")
	AWeaponBase* CurrentWeapon;
	
	UPROPERTY(VisibleInstanceOnly, Category="Inventory")
	TArray<TSubclassOf<AWeaponBase>> WeaponClasses;

	UPROPERTY(VisibleInstanceOnly, Category="Inventory")
	int32 CurrentWeaponIndex;


	const int32 MaxWeaponSlots = 2;
private:
	void SpawnAndEquipWeapon(int32 SlotIndex);
	
};
