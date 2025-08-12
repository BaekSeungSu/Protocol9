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

	/** 외부(게임모드)에서 포커스를 주기 위한 안전한 접근자 */
	UButton* GetRetryButton() const { return Button_Retry; }

protected:
	// UUserWidget
	virtual void NativeConstruct() override;

	// 디자이너 바인딩
	UPROPERTY(meta = (BindWidget)) UTextBlock* TXT_Title = nullptr;
	UPROPERTY(meta = (BindWidget)) UTextBlock* TXT_KillCount = nullptr;
	UPROPERTY(meta = (BindWidget)) UButton*    Button_Retry = nullptr;

	// 버튼 콜백
	UFUNCTION() void HandleRetryClicked();

private:
	void SetTitle(bool bVictory);
	void SetKillCount(int32 InKillCount);

	FSimpleDelegate OnRetry;
};
