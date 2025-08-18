#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UWBP_ResultStats.generated.h"

UCLASS()
class PROTOCOL9_API UUWBP_ResultStats : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SetStats(int32 InKillCount, float InElapsedTime);

	UPROPERTY(meta=(BindWidget)) class UTextBlock* TXT_KillCount;
	UPROPERTY(meta=(BindWidget)) class UTextBlock* TXT_ElapsedTime;

	UPROPERTY(meta=(BindWidget)) class UButton* Btn_Restart;
	UPROPERTY(meta=(BindWidget)) class UButton* Btn_Return;

	UPROPERTY(meta=(BindWidget)) class UButton* Btn_Quit;

protected:
	virtual void NativeConstruct() override;


};
