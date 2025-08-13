#include "UI/UWBP_HUD.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"

void UUWBP_HUD::UpdateTimer(float ElapsedSeconds)
{
	const int32 Minutes = FMath::FloorToInt(ElapsedSeconds / 60.0f);
	const int32 Seconds = FMath::FloorToInt(FMath::Fmod(ElapsedSeconds, 60.0f));

	if (Timer)
	{
		Timer->SetText(FText::FromString(
			FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds)));
	}
}

void UUWBP_HUD::ShowInvincibilityEffect(bool bEnable)
{
	if (Image_Invincibility)
	{
		Image_Invincibility->SetColorAndOpacity(
			bEnable ? FLinearColor(1,1,1,1) : FLinearColor(0.1f,0.1f,0.1f,1));
	}
}

void UUWBP_HUD::ShowSpeedBoostEffect(bool bEnable)
{
	if (Image_SpeedBoost)
	{
		Image_SpeedBoost->SetColorAndOpacity(
			bEnable ? FLinearColor(1,1,1,1) : FLinearColor(0.1f,0.1f,0.1f,1));
	}
}

void UUWBP_HUD::ShowAttackBoostEffect(bool bEnable)
{
	if (Image_AttackBoost)
	{
		Image_AttackBoost->SetColorAndOpacity(
			bEnable ? FLinearColor(1,1,1,1) : FLinearColor(0.1f,0.1f,0.1f,1));
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
		const float Percent = static_cast<float>(CurrentExp) / static_cast<float>(MaxExp);
		EXP->SetPercent(Percent);
	}
}

void UUWBP_HUD::UpdateLevelText(int CharacterLevel)
{
	if (TXT_Level)
	{
		TXT_Level->SetText(FText::FromString(
			FString::Printf(TEXT("LV. %d"), CharacterLevel)));
	}
}

void UUWBP_HUD::UpdateStaminaBar(int CurrentStamina)
{
	TArray<UImage*> StaminaImages = { Image_Stamina1, Image_Stamina2, Image_Stamina3 };

	for (int32 i = 0; i < StaminaImages.Num(); ++i)
	{
		if (StaminaImages[i])
		{
			const FLinearColor Color = (i < CurrentStamina)
				? FLinearColor(1,1,1,1)
				: FLinearColor(0.2f,0.2f,0.2f,1);
			StaminaImages[i]->SetColorAndOpacity(Color);
		}
	}
}

void UUWBP_HUD::UpdateKillCount(int32 KillCount)
{
	if (TXT_Kill)
	{
		TXT_Kill->SetText(FText::FromString(
			FString::Printf(TEXT("Kill : %d"), KillCount)));
	}
}
void UUWBP_HUD::SetWeaponIcon(UTexture2D* Icon)
{
	if (!Image_Weapon) return;

	if (Icon)
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(Icon);
		Image_Weapon->SetBrush(Brush);
		Image_Weapon->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Image_Weapon->SetVisibility(ESlateVisibility::Collapsed);
	}
}
void UUWBP_HUD::UpdateAmmoText(int32 Current, int32 Reserve, bool bInfiniteReserve)
{
	if (!TXT_Ammo) return;
	TXT_Ammo->SetText(FText::FromString(FString::Printf(TEXT("%d/∞"), Current)));
}