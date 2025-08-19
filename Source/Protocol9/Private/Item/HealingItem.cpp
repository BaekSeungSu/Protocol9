#include "Item/HealingItem.h"
#include "Character/HPComponent.h"
#include "Item/ObjectPoolingComponent.h"
#include "Character/MainCharacter.h"

AHealingItem::AHealingItem()
{
	HealAmount = 30.0f;
	ItemType = "HealingItem";
}

void AHealingItem::ActivateItem(AActor* Activator)
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	Super::ActivateItem(Activator);
	GetWorld()->GetTimerManager().ClearTimer(ItemLIfeTimerHandle);
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
		if (OwningPool)
		{
			OwningPool->ReturnObjectToPool(this);
		}
	}
}
