#include "Item/ExperienceItem.h"
#include "Kismet/GameplayStatics.h"
#include "Item/ObjectPoolingComponent.h"
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
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
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
	if (OwningPool)
	{
		OwningPool->ReturnObjectToPool(this);
	}
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
		                                                            //����ġ ���� ���� ���� 
	}
	
	GetWorld()->GetTimerManager().ClearTimer(EffectTimerHandle);
	AffectedPlayer = nullptr;
	DestroyItem();
}
*/