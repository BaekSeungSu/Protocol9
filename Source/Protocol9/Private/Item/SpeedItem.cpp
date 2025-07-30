#include "Item/SpeedItem.h"
#include "Kismet/GameplayStatics.h"
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
	if (Activator && Activator ->ActorHasTag("Player"))
	{
		AMainCharacter* MyCharacter = Cast<AMainCharacter>(Activator);
		if (MyCharacter)
		{
			GEngine->AddOnScreenDebugMessage(-1,
				2.0f,
				FColor::Blue,
				FString::Printf(TEXT("Speed Up"))
				);
			//속도 증가 기능 추가
			AffectedPlayer = MyCharacter; 
			GetWorld()->GetTimerManager().SetTimer(
				EffectTimerHandle,
				this,
				&ASpeedItem::EndEffect,
				ItemDuration,
				false);										
		}
	}
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
}

void ASpeedItem::EndEffect() 
{
	//속도 증가 함수 제거 
	if (AffectedPlayer)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.0f,
			FColor::Red,
			FString::Printf(TEXT("Speed Up Effect End! ")));
		AffectedPlayer = nullptr;
	}
	
	GetWorld()->GetTimerManager().ClearTimer(EffectTimerHandle);
	DestroyItem();
}