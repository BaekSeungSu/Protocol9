#include "UI/UWBP_ResultStats.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UUWBP_ResultStats::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (Btn_Restart) { Btn_Restart->SetClickMethod(EButtonClickMethod::PreciseClick); }
	if (Btn_Return)  { Btn_Return->SetClickMethod(EButtonClickMethod::PreciseClick); }
}

void UUWBP_ResultStats::SetStats(int32 InKillCount, float InElapsedTime)
{
	if (TXT_KillCount)
	{
		TXT_KillCount->SetText(
			FText::FromString(FString::Printf(TEXT("Kills : %d"), InKillCount)));
	}
	if (TXT_ElapsedTime)
	{
		const int32 Min = FMath::FloorToInt(InElapsedTime / 60.f);
		const int32 Sec = FMath::FloorToInt(FMath::Fmod(InElapsedTime, 60.f));
		TXT_ElapsedTime->SetText(
			FText::FromString(FString::Printf(TEXT("Time : %02d:%02d"), Min, Sec)));
	}
}
