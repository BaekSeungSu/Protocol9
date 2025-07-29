#include "Weapons/InventoryComponent.h"

#include "GameFramework/Character.h"
#include "Weapons/WeaponBase.h"

UInventoryComponent::UInventoryComponent()
{

	PrimaryComponentTick.bCanEverTick = false;


}

void UInventoryComponent::AddWeapon(TSubclassOf<AWeaponBase> WeaponClass)
{
	if (WeaponClass == nullptr)
	{
		return;
	}

	ACharacter* OwningCharacter = Cast<ACharacter>(GetOwner());
	if (OwningCharacter == nullptr)
	{
		return;
	}
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwningCharacter;

	AWeaponBase* NewWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, SpawnParams);

	if (NewWeapon)
	{
		NewWeapon->AttachToComponent(OwningCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));
		WeaponSlots.Add(NewWeapon);
		CurrentWeaponIndex = WeaponSlots.Num() - 1;
	}
}

AWeaponBase* UInventoryComponent::GetCurrentWeapon()
{
	return WeaponSlots[CurrentWeaponIndex];
}

void UInventoryComponent::SwapToNextWeapon()
{
}


void UInventoryComponent::SwapWeaponSlots()
{
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
	
}
