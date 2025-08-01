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
	Super::ActivateItem(Activator);
	UE_LOG(LogTemp,Warning,TEXT("Called Healing Item"))
	if (Activator && Activator ->ActorHasTag("Player"))
	{
		UHPComponent* HPComponent = Activator->FindComponentByClass<UHPComponent>();
		if (HPComponent)
		{
			HPComponent->AddHealth(HealAmount);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT(" Error!"));
		}
		DestroyItem();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT(" Error!"));
	}
}
