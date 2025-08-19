#include "Item/ExperienceItem.h"
#include "Kismet/GameplayStatics.h"
#include "Item/ObjectPoolingComponent.h"
#include "Components/SphereComponent.h"

AExperienceItem::AExperienceItem()
//: AffectedPlayer(nullptr)
{
	AddExperience = 20;
	ItemType = "ExperienceItem";
}

void AExperienceItem::ActivateItem(AActor* Activator)
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	Super::ActivateItem(Activator);
	GetWorld()->GetTimerManager().ClearTimer(ItemLIfeTimerHandle);
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
		}
	}
	if (OwningPool)
	{
		OwningPool->ReturnObjectToPool(this);
	}
}
