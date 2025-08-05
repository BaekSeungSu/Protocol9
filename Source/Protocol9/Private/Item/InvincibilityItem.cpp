#include "Item/InvincibilityItem.h"
#include "Character/MainCharacter.h"
#include "Character/HPComponent.h"
#include "Item/ObjectPoolingComponent.h"
#include "Kismet/GameplayStatics.h"

AInvincibilityItem::AInvincibilityItem()
	: AffectedPlayer(nullptr)
{
	ItemDuration = 3.0f;
	ItemType = "InvincibilityItem";
}

void AInvincibilityItem::ActivateItem(AActor* Activator)
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	Super::ActivateItem(Activator);
	if (Activator && Activator ->ActorHasTag("Player"))
	{
		AffectedPlayer = Activator;   
		UHPComponent* HPComponent = Activator->FindComponentByClass<UHPComponent>();
		if (HPComponent)
		{
			GEngine->AddOnScreenDebugMessage(-1,
			2.0f,
			FColor::Blue,
			FString::Printf(TEXT("Invincibility Time! ")));				
			
			HPComponent->LockHealth();								//公利 肺流 眠啊
			
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
			GEngine->AddOnScreenDebugMessage(
			-1,
			2.0f,
			FColor::Red,
			FString::Printf(TEXT("Invincibility Time End! ")));
			HPComponent->UnlockHealth();
		}		// 公利 off 肺流 眠啊                         
	}
	
	GetWorld()->GetTimerManager().ClearTimer(EffectTimerHandle);
	AffectedPlayer = nullptr;
	if (OwningPool)
	{
		OwningPool->ReturnObjectToPool(this);
	}
}