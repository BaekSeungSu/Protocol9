#include "Item/SpeedItem.h"
#include "Kismet/GameplayStatics.h"
#include "Character/ControlComponent.h"
#include "Item/ObjectPoolingComponent.h"
#include "Components/SphereComponent.h"

ASpeedItem::ASpeedItem()
: AffectedPlayer(nullptr)
{
	MultiSpeed = 1.5f;
	ItemDuration = 3.0f;
	ItemType = "SpeedItem";
}

void ASpeedItem::ActivateItem(AActor* Activator)
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	Super::ActivateItem(Activator);
	if (Activator && Activator ->ActorHasTag("Player"))
	{
		AMainCharacter* MyCharacter = Cast<AMainCharacter>(Activator);
		if (MyCharacter)
		{
			UControlComponent* ControlComp= MyCharacter->FindComponentByClass<UControlComponent>();
			if (ControlComp)
			{
				GEngine->AddOnScreenDebugMessage(-1,
					2.0f,
					FColor::Blue,
					FString::Printf(TEXT("Speed Up"))
					);
				ControlComp->AddSpeed(MultiSpeed);
				AffectedPlayer = ControlComp;
				MyCharacter->HandleSpeedBoostEffect(); //UI 추가
				GetWorld()->GetTimerManager().SetTimer(
					EffectTimerHandle,
					this,
					&ASpeedItem::EndEffect,
					ItemDuration,
					false);
			}
		}
	}
}

void ASpeedItem::EndEffect() 
{
	if (AffectedPlayer)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.0f,
			FColor::Red,
			FString::Printf(TEXT("Speed Up Effect End! ")));
		AffectedPlayer->ResetSpeed();
	}
	
	GetWorld()->GetTimerManager().ClearTimer(EffectTimerHandle);
	AffectedPlayer = nullptr;
	if (OwningPool)
	{
		OwningPool->ReturnObjectToPool(this);
	}
}