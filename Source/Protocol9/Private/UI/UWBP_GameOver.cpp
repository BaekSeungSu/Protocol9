#include "UI/UWBP_GameOver.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UWBP_GameOver::NativeConstruct()
{
	Super::NativeConstruct();

	if (Button_Retry)
	{
		Button_Retry->OnClicked.Clear();
		Button_Retry->OnClicked.AddDynamic(this, &UWBP_GameOver::HandleRetryClicked);
	}
}

void UWBP_GameOver::Setup(bool bVictory, int32 InKillCount, FSimpleDelegate InOnRetry)
{
	OnRetry = InOnRetry;
	SetTitle(bVictory);
	SetKillCount(InKillCount);
}

void UWBP_GameOver::SetTitle(bool bVictory)
{
	if (TXT_Title)
	{
		// 문구는 취향대로 바꿔도 OK
		// 실패를 좀 더 "게임 느낌"으로 하고 싶으면 "DEFEATED" 같은 것도 좋음
		const FText TitleText = bVictory ? FText::FromString(TEXT("VICTORY"))
										 : FText::FromString(TEXT("GAME OVER"));
		TXT_Title->SetText(TitleText);
	}
}

void UWBP_GameOver::SetKillCount(int32 InKillCount)
{
	if (TXT_KillCount)
	{
		// 예: "Kills : 37"
		TXT_KillCount->SetText(FText::FromString(FString::Printf(TEXT("Kills : %d"), InKillCount)));
	}
}

void UWBP_GameOver::HandleRetryClicked()
{
	if (OnRetry.IsBound())
	{
		OnRetry.Execute();
	}
}
