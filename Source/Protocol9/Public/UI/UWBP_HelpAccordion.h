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
	
	UPROPERTY(meta=(BindWidget))             UScrollBox*     SB_Content;      
	UPROPERTY(meta=(BindWidget))             UExpandableArea* EA_Controls;
	UPROPERTY(meta=(BindWidget))             UExpandableArea* EA_Items;
	UPROPERTY(meta=(BindWidget))             UExpandableArea* EA_Objectives;
	UPROPERTY(meta=(BindWidgetOptional))     UButton*        Btn_ESC;         

	UFUNCTION() void OnControlsChanged(UExpandableArea* Area, bool bExpanded);
	UFUNCTION() void OnItemsChanged(UExpandableArea* Area, bool bExpanded);
	UFUNCTION() void OnObjectivesChanged(UExpandableArea* Area, bool bExpanded);
	UFUNCTION() void OnEscClicked();

private:
	bool bInternalToggle = false; 

	void SetExpandedExclusive(UExpandableArea* Target);
	void ScrollTo(UWidget* Widget);
	void CloseHelp();
};
