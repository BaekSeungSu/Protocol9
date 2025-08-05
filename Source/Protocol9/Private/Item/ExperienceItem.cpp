#include "Item/ExperienceItem.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"

AExperienceItem::AExperienceItem()
//: AffectedPlayer(nullptr)
{
	AddExperience = 20;
	//ItemDuration = 3.0f;
	ItemType = "ExperienceItem";
}

void AExperienceItem::ActivateItem(AActor* Activator)
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
				FString::Printf(TEXT("Increased experience "))
				);
			MyCharacter ->AddExp(AddExperience);
			//AffectedPlayer = MyCharacter; 

			/*GetWorld()->GetTimerManager().SetTimer(
				EffectTimerHandle,
				this,
				&AExperienceItem::EndEffect,
				ItemDuration,
				false);
				*/										
		}
	}
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
}

/*
void AExperienceItem::EndEffect() 
{
	
	if (AffectedPlayer)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.0f,
			FColor::Red,
			FString::Printf(TEXT("Increased experience End! ")));
		                                                            //경험치 증가 로직 제거 
	}
	
	GetWorld()->GetTimerManager().ClearTimer(EffectTimerHandle);
	AffectedPlayer = nullptr;
	DestroyItem();
}
*/