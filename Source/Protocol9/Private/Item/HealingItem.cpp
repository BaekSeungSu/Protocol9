#include "Item/HealingItem.h"
#include "Character/MainCharacter.h"

AHealingItem::AHealingItem()
{
	HealAmount = 30.0f;
	ItemType = "Healing";
}

void AHealingItem::ActivateItem(AActor* Activator)
{
	if (Activator && Activator ->ActorHasTag("Player"))
	{
		if (AMainCharacter* Character = Cast<AMainCharacter>(Activator))
		{
			GEngine->AddOnScreenDebugMessage(-1,
					2.0f,
					FColor::Red,
					FString::Printf(TEXT("Heal"))
					);
		}
		DestroyItem();
	}
}
