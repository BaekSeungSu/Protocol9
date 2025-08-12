#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UWBP_GameOver.generated.h"

class UTextBlock;
class UButton;

/** GameOver/Victory 공용 위젯 */
UCLASS()
class PROTOCOL9_API UWBP_GameOver : public UUserWidget
{
	GENERATED_BODY()

public:
	/** 화면 세팅: 승리/패배, 킬 수, Retry 콜백 */
	void Setup(bool bVictory, int32 InKillCount, FSimpleDelegate InOnRetry);

	/** 텍스트만 따로 바꿔야 할 때도 사용 가능 */
	void SetKillCount(int32 InKillCount);

protected:
	virtual void NativeConstruct() override;

	/** 디자이너에서 만든 위젯 변수명과 반드시 동일 */
	UPROPERTY(meta = (BindWidget)) UTextBlock* TXT_Title;
	UPROPERTY(meta = (BindWidget)) UTextBlock* TXT_KillCount;
	UPROPERTY(meta = (BindWidget)) UButton*    Button_Retry;

	UFUNCTION() void HandleRetryClicked();

private:
	FSimpleDelegate OnRetry;
	void SetTitle(bool bVictory);
};
