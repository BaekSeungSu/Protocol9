#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LevelUp/LevelUpRow.h" 
#include "LevelUpUserWidget.generated.h"


UCLASS()
class PROTOCOL9_API ULevelUpUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:
 UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void SetLevelUpOptions(const TArray<FLevelUpRow>& ChosenOptions, ULevelUpComponent* LevelUpComponentRef);
};
