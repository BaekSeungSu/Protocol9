#include "Item/HealingItem.h"
#include "Character/HPComponent.h"
#include "Character/MainCharacter.h"

AHealingItem::AHealingItem()
{
	HealAmount = 30.0f;
	ItemType = "HealingItem";
}

void AHealingItem::ActivateItem(AActor* Activator)
{
	if (Activator && Activator ->ActorHasTag("Player"))
	{
		if (AMainCharacter* MainCharacter = Cast<AMainCharacter>(Activator))
		{
			//MainCharacter->AddHealth(HealAmount);
		}
		DestroyItem();
	}
}
