#include "Item/ExperienceItem.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"

AExperienceItem::AExperienceItem()
: AffectedPlayer(nullptr)
{
	MultiExperience = 2.0f;
	ItemDuration = 3.0f;
	ItemType = "SpeedItem";
}

void AExperienceItem::ActivateItem(AActor* Activator)
{
	if (Activator && Activator ->ActorHasTag("Player"))
	{
		AMainCharacter* MyCharacter = Cast<AMainCharacter>(Activator);
		if (MyCharacter)
		{
			GEngine->AddOnScreenDebugMessage(-1,
				2.0f,
				FColor::Blue,
				FString::Printf(TEXT("Increased experience "))
				);
			//속도 증가 기능 추가
			AffectedPlayer = MyCharacter; 
			GetWorld()->GetTimerManager().SetTimer(
				EffectTimerHandle,
				this,
				&AExperienceItem::EndEffect,
				ItemDuration,
				false);										
		}
	}
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
}

void AExperienceItem::EndEffect() 
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