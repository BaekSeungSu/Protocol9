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

