#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "Character/ControlComponent.h"
#include "Character/MainCharacter.h"
#include "SpeedItem.generated.h"



UCLASS()
class PROTOCOL9_API ASpeedItem : public AItemBase
{
	GENERATED_BODY()
public:
	ASpeedItem();
	UPROPERTY()
	UControlComponent* AffectedPlayer;
	
	virtual void EndEffect() override;

	virtual void ActivateItem(AActor* Activator)override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Item")
	float MultiSpeed;
	
};
