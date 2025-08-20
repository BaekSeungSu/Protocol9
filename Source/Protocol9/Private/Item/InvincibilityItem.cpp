#include "Item/InvincibilityItem.h"
#include "Character/MainCharacter.h"
#include "Character/HPComponent.h"
#include "Item/ObjectPoolingComponent.h"
#include "Kismet/GameplayStatics.h"

AInvincibilityItem::AInvincibilityItem()
	: AffectedPlayer(nullptr)
{
	ItemDuration = 5.0f;
	ItemType = "InvincibilityItem";
}

void AInvincibilityItem::ActivateItem(AActor* Activator)
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	Super::ActivateItem(Activator);
	GetWorld()->GetTimerManager().ClearTimer(ItemLIfeTimerHandle);
	if (Activator && Activator ->ActorHasTag("Player"))
	{
		AffectedPlayer = Activator;   
		UHPComponent* HPComponent = Activator->FindComponentByClass<UHPComponent>();
		if (HPComponent)
		{			
			HPComponent->LockHealth();								//���� ���� �߰�

			//UI : 아이템 아이콘 점등 기능 추가
			AMainCharacter* MyCharacter = Cast<AMainCharacter>(Activator);
			if (MyCharacter)
			{
				MyCharacter->HandleInvincibilityEffect();
			}
			
			GetWorld()->GetTimerManager().SetTimer(
			EffectTimerHandle,
			this,
			&AInvincibilityItem::EndEffect,
			ItemDuration,
			false);										
		}
	}
}

void AInvincibilityItem::EndEffect() 
{
	if (AffectedPlayer)
	{
		UHPComponent* HPComponent = AffectedPlayer->FindComponentByClass<UHPComponent>();
		if (HPComponent)
		{
			HPComponent->UnlockHealth();
		}		                
	}
	
	GetWorld()->GetTimerManager().ClearTimer(EffectTimerHandle);
	AffectedPlayer = nullptr;
	if (OwningPool)
	{
		OwningPool->ReturnObjectToPool(this);
	}
}