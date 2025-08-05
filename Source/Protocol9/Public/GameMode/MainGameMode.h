#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainGameMode.generated.h"

class UUWBP_HUD;

UCLASS()
class PROTOCOL9_API AMainGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

protected:
	void UpdateGameTimer();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUWBP_HUD> HUDWidgetClass;

private:
	FTimerHandle GameTimerHandle;
	float ElapsedTime = 0.0f;

	UPROPERTY()
	UUWBP_HUD* HUDWidget;
	
};
