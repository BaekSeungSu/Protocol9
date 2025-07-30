#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "SpeedItem.generated.h"


UCLASS()
class PROTOCOL9_API ASpeedItem : public AItemBase
{
	GENERATED_BODY()
public:
	ASpeedItem();
	
	void ActivateItem(AActor* Activator)override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Item")
	float MultiSpeed;
};
