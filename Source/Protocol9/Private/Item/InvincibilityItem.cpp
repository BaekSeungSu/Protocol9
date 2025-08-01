#include "Item/InvincibilityItem.h"
#include "Character/MainCharacter.h"
#include "Kismet/GameplayStatics.h"
AInvincibilityItem::AInvincibilityItem()
	: AffectedPlayer(nullptr)
{
	ItemDuration = 3.0f;
	ItemType = "InvincibilityItem";
}

void AInvincibilityItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);
	if (Activator && Activator ->ActorHasTag("Player"))
	{
		AMainCharacter* MyCharacter = Cast<AMainCharacter>(Activator);
		if (MyCharacter)
		{
			GEngine->AddOnScreenDebugMessage(-1,
			2.0f,
			FColor::Blue,
			FString::Printf(TEXT("Invincibility Time! ")));				
			
			//MyCharacter->LockHelath();								//���� ���� �߰� 
			
			AffectedPlayer = MyCharacter;                       //�� ȿ���� �޴� ���� ���� 
			GetWorld()->GetTimerManager().SetTimer(
			EffectTimerHandle,
			this,
			&AInvincibilityItem::EndEffect,
			ItemDuration,
			false);										
		}
	}
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
}

void AInvincibilityItem::EndEffect() 
{
	if (AffectedPlayer)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.0f,
			FColor::Red,
			FString::Printf(TEXT("Invincibility Time End! ")));
		//AffectedPlayer->UnlockHelath();                     // ���� off ���� �߰�                         
	}
	
	GetWorld()->GetTimerManager().ClearTimer(EffectTimerHandle);
	AffectedPlayer = nullptr;
	DestroyItem();
}