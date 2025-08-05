#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UWBP_HUD.generated.h"

UCLASS()
class PROTOCOL9_API UUWBP_HUD : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void UpdateTimer(float ElapsedSeconds);

protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Timer;
};
