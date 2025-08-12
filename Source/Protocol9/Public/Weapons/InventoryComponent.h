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
	void EquipWeaponAtIndex(int32 SlotIndex);
	bool HasWeaponInSlot(int32 SlotIndex) const;
	
	UAnimMontage* GetEquipMontageForSlot(int32 SlotIndex) const;
	int32 GetCurrentWeaponIndex() const {return CurrentWeaponIndex;}
	AWeaponBase* GetCurrentWeapon() const{return CurrentWeapon;};
	
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
