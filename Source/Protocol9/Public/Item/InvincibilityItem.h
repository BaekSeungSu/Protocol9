#pragma once

#include "CoreMinimal.h"
#include "Item/ItemBase.h"
#include "Character/MainCharacter.h"
#include "InvincibilityItem.generated.h"


UCLASS()
class PROTOCOL9_API AInvincibilityItem : public AItemBase
{
	GENERATED_BODY()
public:
	AInvincibilityItem();

	UPROPERTY()
	AMainCharacter* AffectedPlayer;

	virtual void EndEffect() override;

	virtual void ActivateItem(AActor* Activator)override;

	
};
