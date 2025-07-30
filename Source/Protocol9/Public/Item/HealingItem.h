#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "HealingItem.generated.h"


UCLASS()
class PROTOCOL9_API AHealingItem : public AItemBase
{
	GENERATED_BODY()
public:
	AHealingItem();
	virtual void ActivateItem(AActor* Activator)override;
	
	
	UPROPERTY(EditAnywhere,BLUEprintReadWrite,Category="Item")
	float HealAmount;
};
