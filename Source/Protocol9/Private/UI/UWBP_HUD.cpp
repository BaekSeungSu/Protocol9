#include "UI/UWBP_HUD.h"
#include "Components/TextBlock.h"

void UUWBP_HUD::UpdateTimer(float ElapsedSeconds)
{
	UE_LOG(LogTemp, Warning, TEXT("UpdateTimer called. ElapsedSeconds: %f"), ElapsedSeconds);

	int32 Minutes = FMath::FloorToInt(ElapsedSeconds / 60.0f);
	int32 Seconds = FMath::FloorToInt(FMath::Fmod(ElapsedSeconds, 60.0f));

	FString TimerString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
	if (Timer)
	{
		Timer->SetText(FText::FromString(TimerString));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Timer is null"));
	}
}

void UUWBP_HUD::ShowInvincibilityEffect(bool bEnable)
{
	if (Image_Invincibility)
	{
		FLinearColor Color = bEnable ? FLinearColor(1, 1, 1, 1) : FLinearColor(0.1, 0.1, 0.1, 1);
		Image_Invincibility->SetColorAndOpacity(Color);
	}
}

void UUWBP_HUD::ShowSpeedBoostEffect(bool bEnable)
{
	if (Image_SpeedBoost)
	{
		UE_LOG(LogTemp, Warning, TEXT("Image_SpeedBoost is valid."));
		FLinearColor Color = bEnable ? FLinearColor(1, 1, 1, 1) : FLinearColor(0.1, 0.1, 0.1, 1);
		Image_SpeedBoost->SetColorAndOpacity(Color);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Image_SpeedBoost is NULL!"));
	}
}

void UUWBP_HUD::ShowAttackBoostEffect(bool bEnable)
{
	if (Image_AttackBoost)
	{
		FLinearColor Color = bEnable ? FLinearColor(1, 1, 1, 1) : FLinearColor(0.1, 0.1, 0.1, 1);
		Image_AttackBoost->SetColorAndOpacity(Color);
	}
}
void UUWBP_HUD::UpdateHPBar(float CurrentHP, float MaxHP)
{
	if (HP)
	{
		HP->SetPercent(CurrentHP / MaxHP);
	}
}
void UUWBP_HUD::UpdateEXPBar(int CurrentExp, int MaxExp)
{
	if (EXP && MaxExp > 0)
	{
		float Percent = static_cast<float>(CurrentExp) / static_cast<float>(MaxExp);
		EXP->SetPercent(Percent);
	}
}
void UUWBP_HUD::UpdateLevelText(int CharacterLevel)
{
	if (TXT_Level)
	{
		FString LevelStr = FString::Printf(TEXT("LV. %d"), CharacterLevel);
		TXT_Level->SetText(FText::FromString(LevelStr));
	}
}
void UUWBP_HUD::UpdateStaminaBar(int CurrentStamina)
{
	TArray<UImage*> StaminaImages = {
		Image_Stamina1, Image_Stamina2, Image_Stamina3
	};

	for (int i = 0; i < StaminaImages.Num(); ++i)
	{
		if (StaminaImages[i])
		{
			FLinearColor Color = (i < CurrentStamina)
				? FLinearColor(1, 1, 1, 1)          
				: FLinearColor(0.2f, 0.2f, 0.2f, 1);
			
			StaminaImages[i]->SetColorAndOpacity(Color);
		}
	}
}
