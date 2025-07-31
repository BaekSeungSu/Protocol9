#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Character/MainCharacter.h"
#include "HPComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROTOCOL9_API UHPComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHPComponent();

protected:
	
	virtual void BeginPlay() override;
};
