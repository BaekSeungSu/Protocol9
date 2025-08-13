#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "UWBP_HUD.generated.h"

class UImage; 
class UTexture2D;

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
	UFUNCTION(BlueprintCallable)
	void UpdateHPBar(float CurrentHP, float MaxHP);
	UFUNCTION(BlueprintCallable)
	void UpdateEXPBar(int CurrentExp, int MaxExp);
	UFUNCTION(BlueprintCallable)
	void UpdateLevelText(int CharacterLevel);
	UFUNCTION(BlueprintCallable)
	void UpdateStaminaBar(int CurrentStamina);
	UFUNCTION(BlueprintCallable)
	void UpdateKillCount(int32 KillCount);

	UFUNCTION(BlueprintCallable)
	void UpdateAmmoText(int32 Current, int32 Reserve, bool bInfiniteReserve);
	
	UFUNCTION(BlueprintCallable)
    void SetWeaponIcon(UTexture2D* Icon);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Timer;
	UPROPERTY(meta = (BindWidget))
	UImage* Image_Invincibility;

	UPROPERTY(meta = (BindWidget))
	UImage* Image_SpeedBoost;

	UPROPERTY(meta = (BindWidget))
	UImage* Image_AttackBoost;
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HP;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* EXP;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TXT_Level;

	UPROPERTY(meta = (BindWidget)) 
	UImage* Image_Stamina1;
	UPROPERTY(meta = (BindWidget)) 
	UImage* Image_Stamina2;
	UPROPERTY(meta = (BindWidget)) 
	UImage* Image_Stamina3;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TXT_Kill;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TXT_Ammo;

	UPROPERTY(meta=(BindWidget)) UImage* Image_Weapon = nullptr;



};
