#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h" 
#include "UWBP_HUD.generated.h"

UCLASS()
class PROTOCOL9_API UUWBP_HUD : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void UpdateTimer(float ElapsedSeconds);
	UFUNCTION(BlueprintCallable)
	void ShowInvincibilityEffect(bool bEnable);
	UFUNCTION(BlueprintCallable)
	void ShowSpeedBoostEffect(bool bEnable);
	UFUNCTION(BlueprintCallable)
	void ShowAttackBoostEffect(bool bEnable);


protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Timer;
	UPROPERTY(meta = (BindWidget))
	UImage* Image_Invincibility;

	UPROPERTY(meta = (BindWidget))
	UImage* Image_SpeedBoost;

	UPROPERTY(meta = (BindWidget))
	UImage* Image_AttackBoost;

};
