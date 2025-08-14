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

	if (WeaponClasses.Num() < MaxWeaponSlots)
	{
		WeaponClasses.Init(nullptr, MaxWeaponSlots);
	}

	const int32 EmptySlotIndex = WeaponClasses.Find(nullptr);
	if (EmptySlotIndex != INDEX_NONE)
	{
		WeaponClasses[EmptySlotIndex] = WeaponClass;
		EquipWeaponAtIndex(EmptySlotIndex);
	}
	
}

void UInventoryComponent::EquipWeaponAtIndex(int32 SlotIndex)
{
	if (!WeaponClasses.IsValidIndex(SlotIndex)
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

bool UInventoryComponent::AddWeaponToSlot(TSubclassOf<AWeaponBase> WeaponClass, int32 SlotIndex, bool bEquipImmediately)
{
	if (!WeaponClass) return false;
	if (WeaponClasses.Num() < MaxWeaponSlots)
	{
		WeaponClasses.Init(nullptr, MaxWeaponSlots);
	}

	if (!WeaponClasses.IsValidIndex(SlotIndex)) return false;

	WeaponClasses[SlotIndex] = WeaponClass;
	if (bEquipImmediately)
	{
		EquipWeaponAtIndex(SlotIndex);
	}
	return true;
}

bool UInventoryComponent::AddOrReplaceWeapon(TSubclassOf<AWeaponBase> WeaponClass)
{
	if (!WeaponClass) return false;

	if (WeaponClasses.Num() < MaxWeaponSlots)
	{
		WeaponClasses.Init(nullptr, MaxWeaponSlots);
	}
	
	int32 Count = 0;
	for (int32 i = 0; i < MaxWeaponSlots; ++i)
		if (WeaponClasses[i] != nullptr) ++Count;

	int32 TargetSlot = INDEX_NONE;

	if (Count == 0)
	{
		TargetSlot = 0;
	}
	else if (Count == 1)
	{
		if (WeaponClasses.IsValidIndex(1) && WeaponClasses[1] == nullptr) TargetSlot = 1;
		else if (WeaponClasses.IsValidIndex(0) && WeaponClasses[0] == nullptr) TargetSlot = 0;
	}
	else
	{
		TargetSlot = (CurrentWeaponIndex >= 0 && CurrentWeaponIndex < MaxWeaponSlots) ? CurrentWeaponIndex : 0;
	}

	if (TargetSlot == INDEX_NONE)
	{
		TargetSlot = (CurrentWeaponIndex >= 0 && CurrentWeaponIndex < MaxWeaponSlots) ? CurrentWeaponIndex : 0;
	}

	WeaponClasses[TargetSlot] = WeaponClass;
	EquipWeaponAtIndex(TargetSlot);
	return true;
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
			WeaponChanged.Broadcast(GetCurrentWeaponType());

			if (CurrentWeapon)
			{
				CurrentWeapon->SetOwningCharacter(Cast<AMainCharacter>(OwningCharacter));
				CurrentWeapon->LoadWeaponData();
				UGameplayStatics::FinishSpawningActor(CurrentWeapon, SpawnTransform);
				CurrentWeapon->AttachToOwnerSocket();
				CurrentWeaponIndex = SlotIndex;
				WeaponChanged.Broadcast(GetCurrentWeaponType());
			}
		}
	}
}

EWeaponType UInventoryComponent::GetCurrentWeaponType() const
{
	if (CurrentWeapon && CurrentWeapon->CurrentWeaponData)
	{
		return CurrentWeapon->CurrentWeaponData->WeaponType;
	}
	return EWeaponType::Pistol;
}
