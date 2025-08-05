#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "CrosshairWidget.generated.h"

UCLASS()
class PROTOCOL9_API UCrosshairWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UImage* LineTop;
	UPROPERTY(meta = (BindWidget))
	UImage* LineBottom;
	UPROPERTY(meta = (BindWidget))
	UImage* LineLeft;
	UPROPERTY(meta = (BindWidget))
	UImage* LineRight;

	UFUNCTION(BlueprintCallable, Category = "Crosshair")
	void UpdateCrosshairSize(const float Speed);
};
