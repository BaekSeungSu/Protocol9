#pragma once
#include "Blueprint/UserWidget.h"
#include "UWBP_HelpAccordion.generated.h"

class UScrollBox;
class UExpandableArea;
class UButton;

UCLASS()
class PROTOCOL9_API UUWBP_HelpAccordion : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

	// WBP_Help 내 위젯 이름과 반드시 동일해야 함
	UPROPERTY(meta=(BindWidget))             UScrollBox*     SB_Content;      // ← 이름 주의
	UPROPERTY(meta=(BindWidget))             UExpandableArea* EA_Controls;
	UPROPERTY(meta=(BindWidget))             UExpandableArea* EA_Items;
	UPROPERTY(meta=(BindWidget))             UExpandableArea* EA_Objectives;
	UPROPERTY(meta=(BindWidgetOptional))     UButton*        Btn_ESC;         // 상단 X/ESC 버튼

	UFUNCTION() void OnControlsChanged(UExpandableArea* Area, bool bExpanded);
	UFUNCTION() void OnItemsChanged(UExpandableArea* Area, bool bExpanded);
	UFUNCTION() void OnObjectivesChanged(UExpandableArea* Area, bool bExpanded);
	UFUNCTION() void OnEscClicked();

private:
	bool bInternalToggle = false; // 내부에서 접기/펼치기할 때 재귀 콜백 방지

	void SetExpandedExclusive(UExpandableArea* Target);
	void ScrollTo(UWidget* Widget);
	void CloseHelp();
};
