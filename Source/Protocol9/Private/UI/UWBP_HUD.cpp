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
