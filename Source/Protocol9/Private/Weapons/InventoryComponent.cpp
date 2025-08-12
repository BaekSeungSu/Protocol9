#include "Weapons/InventoryComponent.h"
#include "Character/MainCharacter.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Weapons/WeaponBase.h"
#include "Weapons/Data/WeaponData.h"

UInventoryComponent::UInventoryComponent()
{

	PrimaryComponentTick.bCanEverTick = false;

	CurrentWeapon = nullptr;
	CurrentWeaponIndex = -1;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	WeaponClasses.Init(nullptr, MaxWeaponSlots);
}


void UInventoryComponent::AddWeapon(TSubclassOf<AWeaponBase> WeaponClass)
{
	if (!WeaponClass) return;
	
	int32 EmptySlotIndex = WeaponClasses.Find(nullptr);
	if (EmptySlotIndex != INDEX_NONE)
	{
		WeaponClasses[EmptySlotIndex] = WeaponClass;
		EquipWeaponAtIndex(EmptySlotIndex);
	}
	else
	{
		WeaponClasses[CurrentWeaponIndex] = WeaponClass;
		EquipWeaponAtIndex(CurrentWeaponIndex);
	}
	
}

void UInventoryComponent::EquipWeaponAtIndex(int32 SlotIndex)
{
	if (!WeaponClasses.IsValidIndex(SlotIndex)
		|| CurrentWeaponIndex == SlotIndex
		|| WeaponClasses[SlotIndex] == nullptr)
	{
		return;
	}
	
	SpawnAndEquipWeapon(SlotIndex);

	
}

bool UInventoryComponent::HasWeaponInSlot(int32 SlotIndex) const
{
	if (!WeaponClasses.IsValidIndex(SlotIndex))
	{
		return false;
	}

	return WeaponClasses[SlotIndex] != nullptr;
}

UAnimMontage* UInventoryComponent::GetEquipMontageForSlot(int32 SlotIndex) const
{
	if (!WeaponClasses.IsValidIndex(SlotIndex) || WeaponClasses[SlotIndex] == nullptr)
	{
		return nullptr;
	}
	
	AWeaponBase* WeaponCDO = WeaponClasses[SlotIndex]->GetDefaultObject<AWeaponBase>();
	if (WeaponCDO == nullptr)
	{
		return nullptr;
	}
	
	UDataTable* DataTable = WeaponCDO->GetWeaponDataTable(); 
	FName RowName = WeaponCDO->GetWeaponDataRowName();
	
	if (DataTable && !RowName.IsNone())
	{
		const FWeaponData* WeaponData = DataTable->FindRow<FWeaponData>(RowName, TEXT(""));
		if (WeaponData)
		{
			return WeaponData->EquipMontage;
		}
	}

	return nullptr;
}


void UInventoryComponent::SpawnAndEquipWeapon(int32 SlotIndex)
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}

	TSubclassOf<AWeaponBase> WeaponClassToSpawn = WeaponClasses[SlotIndex];
	if (WeaponClassToSpawn)
	{
		ACharacter* OwningCharacter = Cast<ACharacter>(GetOwner());
		if (OwningCharacter)
		{
			const FTransform SpawnTransform = OwningCharacter->GetMesh()->GetSocketTransform(TEXT("WeaponSocket"));
			CurrentWeapon = GetWorld()->SpawnActorDeferred<AWeaponBase>(WeaponClassToSpawn, SpawnTransform, OwningCharacter);

			if (CurrentWeapon)
			{
				CurrentWeapon->SetOwningCharacter(Cast<AMainCharacter>(OwningCharacter));
				CurrentWeapon->LoadWeaponData();
				UGameplayStatics::FinishSpawningActor(CurrentWeapon, SpawnTransform);
				CurrentWeapon->AttachToOwnerSocket();
				CurrentWeaponIndex = SlotIndex;
			}
		}
	}
}
