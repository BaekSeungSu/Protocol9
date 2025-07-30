#include "Item/HealingItem.h"

AHealingItem::AHealingItem()
{
	HealAmount = 30.0f;
	ItemType = "Healing";
}

void AHealingItem::ActivateItem(AActor* Activator)
{
	if (Activator && Activator ->ActorHasTag("Player"))
	{
		DestroyItem();
	}
}
