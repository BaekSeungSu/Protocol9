#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerUIComponent.generated.h"


class UCrosshairWidget;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROTOCOL9_API UPlayerUIComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlayerUIComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UCrosshairWidget> CrosshairWidgetClass;
	
	float GetAimSize() const { return CurrentAimSize; }
	void SetAimSize(float NewAimSize) { CurrentAimSize = NewAimSize; }

	UFUNCTION(BlueprintCallable, Category="Crosshair")
	void ShowCrosshair();

	UFUNCTION(BlueprintCallable, Category="Crosshair")
	void HideCrosshair();
	
private:
	UPROPERTY()
	UCrosshairWidget* CrosshairWidgetInstance;

	UPROPERTY(EditDefaultsOnly, Category = "UI|Crosshair")
	float DefaultAimSize;
	UPROPERTY(EditDefaultsOnly, Category = "UI|Crosshair")
	float JumpingAimSize;
	UPROPERTY(EditDefaultsOnly, Category = "UI|Crosshair")
	float MovingAimSize;
	UPROPERTY(EditDefaultsOnly, Category = "UI|Crosshair")
	float AimInterpolationSpeed;
	
	float CurrentAimSize;
	float TargetAimSize;

	void InitializeCrosshair();
	void UpdateCrosshair(float DeltaTime);
		
};
