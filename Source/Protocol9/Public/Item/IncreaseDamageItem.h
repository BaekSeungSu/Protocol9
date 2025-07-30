#pragma once

#include "CoreMinimal.h"
#include "Item/ItemBase.h"
#include "Character/MainCharacter.h"
#include "IncreaseDamageItem.generated.h"


UCLASS()
class PROTOCOL9_API AIncreaseDamageItem : public AItemBase
{
	GENERATED_BODY()
public:
	AIncreaseDamageItem();
	UPROPERTY()
	AMainCharacter* AffectedPlayer;

	virtual void EndEffect() override;

	virtual void ActivateItem(AActor* Activator)override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Item")
	float MultiDamage;
};
