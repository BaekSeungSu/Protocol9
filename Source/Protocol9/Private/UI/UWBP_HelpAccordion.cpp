#include "UI/UWBP_HelpAccordion.h"
#include "Components/ScrollBox.h"
#include "Components/ExpandableArea.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "GameMode/MainGameMode.h"

void UUWBP_HelpAccordion::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (EA_Controls)
        EA_Controls->OnExpansionChanged.AddDynamic(this, &UUWBP_HelpAccordion::OnControlsChanged);
    if (EA_Items)
        EA_Items->OnExpansionChanged.AddDynamic(this, &UUWBP_HelpAccordion::OnItemsChanged);
    if (EA_Objectives)
        EA_Objectives->OnExpansionChanged.AddDynamic(this, &UUWBP_HelpAccordion::OnObjectivesChanged);
    if (Btn_ESC)
        Btn_ESC->OnClicked.AddDynamic(this, &UUWBP_HelpAccordion::OnEscClicked);
}

void UUWBP_HelpAccordion::NativeConstruct()
{
    Super::NativeConstruct();

    // 처음엔 Controls만 펼치기
    bInternalToggle = true;
    if (EA_Controls)    EA_Controls->SetIsExpanded(true);
    if (EA_Items)       EA_Items->SetIsExpanded(false);
    if (EA_Objectives)  EA_Objectives->SetIsExpanded(false);
    bInternalToggle = false;

    ScrollTo(EA_Controls);

    // ESC 입력 받을 수 있게
    SetIsFocusable(true);
}

FReply UUWBP_HelpAccordion::NativeOnKeyDown(const FGeometry& G, const FKeyEvent& E)
{
    if (E.GetKey() == EKeys::Escape)
    {
        CloseHelp();
        return FReply::Handled();
    }
    return Super::NativeOnKeyDown(G, E);
}

void UUWBP_HelpAccordion::OnControlsChanged(UExpandableArea* /*Area*/, bool bExpanded)
{
    if (!bExpanded || bInternalToggle) return;
    SetExpandedExclusive(EA_Controls);
}

void UUWBP_HelpAccordion::OnItemsChanged(UExpandableArea* /*Area*/, bool bExpanded)
{
    if (!bExpanded || bInternalToggle) return;
    SetExpandedExclusive(EA_Items);
}

void UUWBP_HelpAccordion::OnObjectivesChanged(UExpandableArea* /*Area*/, bool bExpanded)
{
    if (!bExpanded || bInternalToggle) return;
    SetExpandedExclusive(EA_Objectives);
}

void UUWBP_HelpAccordion::OnEscClicked()
{
    CloseHelp();
}

void UUWBP_HelpAccordion::SetExpandedExclusive(UExpandableArea* Target)
{
    if (!Target) return;

    bInternalToggle = true;
    if (EA_Controls   && EA_Controls   != Target) EA_Controls->SetIsExpanded(false);
    if (EA_Items      && EA_Items      != Target) EA_Items->SetIsExpanded(false);
    if (EA_Objectives && EA_Objectives != Target) EA_Objectives->SetIsExpanded(false);
    if (!Target->GetIsExpanded()) Target->SetIsExpanded(true);
    bInternalToggle = false;

    ScrollTo(Target);
}

void UUWBP_HelpAccordion::ScrollTo(UWidget* Widget)
{
    if (SB_Content && Widget)
    {
        SB_Content->ScrollWidgetIntoView(
            Widget, /*Animate=*/true,
            EDescendantScrollDestination::TopOrLeft, /*Padding=*/0.f
        );
    }
}

void UUWBP_HelpAccordion::CloseHelp()
{
    if (AMainGameMode* GM = Cast<AMainGameMode>(UGameplayStatics::GetGameMode(this)))
    {
        GM->HideHelp();
    }
}


