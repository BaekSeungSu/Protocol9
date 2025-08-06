#pragma once

#include "CoreMinimal.h"
#include "Item/ItemBase.h"
#include "Character/MainCharacter.h"
#include "ExperienceItem.generated.h"


UCLASS()
class PROTOCOL9_API AExperienceItem : public AItemBase
{
	GENERATED_BODY()
public:
	AExperienceItem();

	//UPROPERTY()
	//AMainCharacter* AffectedPlayer;

	//virtual void EndEffect() override;

	virtual void ActivateItem(AActor* Activator)override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Item")
	int AddExperience;
};
