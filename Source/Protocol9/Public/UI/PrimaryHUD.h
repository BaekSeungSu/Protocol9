#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PrimaryHUD.generated.h"

UCLASS()
class PROTOCOL9_API APrimaryHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> MainHUDWidgetClass;

private:
	UPROPERTY()
	UUserWidget* MainHUDWidget;
};
