#include "GameMode/MainGameMode.h"

#include "EngineUtils.h"
#include "UI/UWBP_HUD.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Character/MainCharacter.h"
#include "Character/HPComponent.h"
#include "Enemy/MonsterSpawner.h"
#include "Enemy/MonsterBase.h"
#include "GameInstance/MainGameInstance.h"
#include "UI/UWBP_ResultStats.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"
#include "Components/Button.h"
#include "Map/MinimapCapture.h"
#include "UI/PlayerUIComponent.h"
#include "UI/UWBP_GameOver.h"
#include "UI/UWBP_HelpAccordion.h"

namespace
{
    inline void FlushAllPressedKeys(APlayerController* PC)
    {
        if (PC) { PC->FlushPressedKeys(); }
    }
}

void AMainGameMode::BeginPlay()
{
    Super::BeginPlay();

    const FString MapName = GetWorld()->GetMapName();

    if (MapName.Contains(TEXT("MenuLevel")))
    {
        if (UMainGameInstance* GI = GetGameInstance<UMainGameInstance>())
        {
            if (GI->bPendingResults)
            {
                GI->bPendingResults = false;
                KillCount   = GI->LastKillCount;
                ElapsedTime = GI->LastElapsedTime;
                ShowResultStats();
                return;
            }
        }
        ShowPressAnyKey();
    }
    else if (MapName.Contains(TEXT("TestMap")))
    {
        ShowHUD();
    }
}

void AMainGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    GetWorldTimerManager().ClearTimer(GameTimerHandle);
    GetWorldTimerManager().ClearTimer(PauseTimerHandle);
    Super::EndPlay(EndPlayReason);
}

// ---------- 메뉴/HUD ----------

void AMainGameMode::ShowPressAnyKey()
{
    if (CurrentWidget)
    {
        CurrentWidget->RemoveFromParent();
        CurrentWidget = nullptr;
    }

    UGameplayStatics::SetGamePaused(this, true);

    if (!WBP_PressAnyKey) return;

    CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), WBP_PressAnyKey);
    if (!CurrentWidget) return;

    CurrentWidget->AddToViewport();

    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        PC->bShowMouseCursor      = true;
        PC->bEnableClickEvents    = true;
        PC->bEnableMouseOverEvents= true;

        FInputModeUIOnly Mode;
        Mode.SetWidgetToFocus(CurrentWidget->TakeWidget());
        Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(Mode);

        FlushAllPressedKeys(PC);
    }
}

void AMainGameMode::ShowMainMenu()
{
    if (CurrentWidget)
    {
        CurrentWidget->RemoveFromParent();
        CurrentWidget = nullptr;
    }
    if (AMainCharacter* MC = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
    {
        if (UPlayerUIComponent* UI = MC->FindComponentByClass<UPlayerUIComponent>())
        {
            UI->HideCrosshair();
        }
    }

    UGameplayStatics::SetGamePaused(this, false);
    GetWorldTimerManager().ClearTimer(GameTimerHandle);
    GetWorldTimerManager().ClearTimer(PauseTimerHandle);

    if (!WBP_MainMenu) return;

    CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), WBP_MainMenu);
    if (!CurrentWidget) return;

    CurrentWidget->AddToViewport();

    if (UButton* HelpBtn = Cast<UButton>(CurrentWidget->GetWidgetFromName(TEXT("Btn_Help"))))
    {
        HelpBtn->OnClicked.Clear();
        HelpBtn->OnClicked.AddDynamic(this, &AMainGameMode::OnHelpButtonClicked);
        HelpBtn->SetClickMethod(EButtonClickMethod::PreciseClick);
    }

    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        PC->bShowMouseCursor      = true;
        PC->bEnableClickEvents    = true;
        PC->bEnableMouseOverEvents= true;

        FInputModeUIOnly Mode;
        Mode.SetWidgetToFocus(CurrentWidget->TakeWidget());
        Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(Mode);

        FlushAllPressedKeys(PC);
    }
}

void AMainGameMode::ShowHUD()
{
    if (CurrentWidget)
        CurrentWidget->RemoveFromParent();

    if (!WBP_HUD) return;

    HUDWidget = CreateWidget<UUWBP_HUD>(GetWorld(), WBP_HUD);
    if (!HUDWidget) return;

    for (TActorIterator<AMiniMapCapture> It(GetWorld()); It; ++It)
    {
        It->SetHUD(HUDWidget);
    }

    HUDWidget->AddToViewport();
    CurrentWidget = HUDWidget;

    ElapsedTime = 0.0f;
    GetWorldTimerManager().SetTimer(GameTimerHandle, this, &AMainGameMode::UpdateGameTimer, 1.0f, true);

    NotifyCharacterHUDReady();

    KillCount = 0;
    HUDWidget->UpdateKillCount(KillCount);
    BindSpawnerForKillCount();

    if (AMainCharacter* MC = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
    {
        if (UHPComponent* HP = MC->GetHPComponent())
        {
            HP->OnDeathEvent.AddDynamic(this, &AMainGameMode::HandlePlayerDeath);
        }
        if (UPlayerUIComponent* UI = MC->FindComponentByClass<UPlayerUIComponent>())
        {
            UI->ShowCrosshair();
        }
    }
}

void AMainGameMode::StartGame()
{
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        PC->bShowMouseCursor      = false;
        PC->bEnableClickEvents    = false;
        PC->bEnableMouseOverEvents= false;

        FInputModeGameOnly Mode;
        PC->SetInputMode(Mode);
    }

    UGameplayStatics::OpenLevel(this, FName("TestMap"));
}

// ---------- HUD 보조 ----------

void AMainGameMode::UpdateGameTimer()
{
    ElapsedTime += 1.0f;
    if (HUDWidget) { HUDWidget->UpdateTimer(ElapsedTime); }
}

void AMainGameMode::NotifyCharacterHUDReady()
{
    if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0))
    {
        if (AMainCharacter* MC = Cast<AMainCharacter>(PlayerPawn))
        {
            MC->CacheHUD();
        }
    }
}

void AMainGameMode::AddKillCount()
{
    ++KillCount;
    if (HUDWidget) { HUDWidget->UpdateKillCount(KillCount); }
}

void AMainGameMode::BindSpawnerForKillCount()
{
    if (bKillBindingInitialized) return;

    TArray<AActor*> Spawners;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMonsterSpawner::StaticClass(), Spawners);
    for (AActor* A : Spawners)
        if (AMonsterSpawner* S = Cast<AMonsterSpawner>(A))
            S->OnMonsterSpawned.AddDynamic(this, &AMainGameMode::OnMonsterSpawnedForUI);

    TArray<AActor*> Existing;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMonsterBase::StaticClass(), Existing);
    for (AActor* M : Existing)
        if (AMonsterBase* Monster = Cast<AMonsterBase>(M))
            Monster->OnMonsterDead.AddDynamic(this, &AMainGameMode::OnMonsterDeadForUI);

    bKillBindingInitialized = true;
}

void AMainGameMode::OnMonsterSpawnedForUI(AMonsterBase* Monster)
{
    if (Monster)
        Monster->OnMonsterDead.AddDynamic(this, &AMainGameMode::OnMonsterDeadForUI);
}

void AMainGameMode::OnMonsterDeadForUI(AMonsterBase* /*Monster*/)
{
    AddKillCount();
}

// ---------- 게임오버/결과 ----------

void AMainGameMode::ShowGameOver(bool bVictory, int32 InKillCount)
{
    if (CurrentWidget) CurrentWidget->RemoveFromParent();
    if (AMainCharacter* MC = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
    {
        if (UPlayerUIComponent* UI = MC->FindComponentByClass<UPlayerUIComponent>())
        {
            UI->HideCrosshair();
        }
    }

    if (!WBP_GameOver) return;

    if (UWBP_GameOver* Screen = CreateWidget<UWBP_GameOver>(GetWorld(), WBP_GameOver))
    {
        Screen->AddToViewport();
        Screen->Setup(bVictory, InKillCount, FSimpleDelegate::CreateUObject(this, &AMainGameMode::OnRetryClicked));
        CurrentWidget = Screen;

        if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
        {
            PC->bShowMouseCursor      = true;
            PC->bEnableClickEvents    = true;
            PC->bEnableMouseOverEvents= true;

            FInputModeUIOnly Mode;
            if (UButton* RetryButton = Screen->GetRetryButton())
                Mode.SetWidgetToFocus(RetryButton->TakeWidget());
            else
                Mode.SetWidgetToFocus(Screen->TakeWidget());

            PC->SetInputMode(Mode);

            FlushAllPressedKeys(PC);
        }

        GetWorldTimerManager().ClearTimer(PauseTimerHandle);
        GetWorldTimerManager().SetTimer(
            PauseTimerHandle,
            FTimerDelegate::CreateLambda([this]()
            {
                if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
                {
                    UGameplayStatics::SetGamePaused(this, true);
                }
            }),
            1.5f, false);
    }
}

void AMainGameMode::OnRetryClicked()
{
    GetWorldTimerManager().ClearTimer(PauseTimerHandle);

    if (UMainGameInstance* GI = GetGameInstance<UMainGameInstance>())
    {
        const bool bWinCached = false;
        GI->SetFinalStats(KillCount, ElapsedTime, bWinCached);
    }

    UGameplayStatics::SetGamePaused(this, false);
    if (CurrentWidget) { CurrentWidget->RemoveFromParent(); CurrentWidget = nullptr; }
    UGameplayStatics::OpenLevel(this, FName(TEXT("MenuLevel")));
}

void AMainGameMode::HandlePlayerDeath()
{
    GetWorldTimerManager().ClearTimer(GameTimerHandle);
    ShowGameOver(false, KillCount);
}

void AMainGameMode::HandleBossDefeated()
{
    GetWorldTimerManager().ClearTimer(GameTimerHandle);
    ShowGameOver(true, KillCount);
}

void AMainGameMode::PostShowGameOver()
{
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        PC->SetShowMouseCursor(true);
        FInputModeUIOnly Mode;
        PC->SetInputMode(Mode);
    }
    UGameplayStatics::SetGamePaused(this, true);
}

void AMainGameMode::OnAnyKeyPressed()
{
    ShowMainMenu();
}

void AMainGameMode::ShowResultStats()
{
    if (CurrentWidget)
    {
        CurrentWidget->RemoveFromParent();
        CurrentWidget = nullptr;
    }
    if (AMainCharacter* MC = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
    {
        if (UPlayerUIComponent* UI = MC->FindComponentByClass<UPlayerUIComponent>())
        {
            UI->HideCrosshair();
        }
    }
    if (!WBP_ResultStats) return;

    if (UUWBP_ResultStats* Stats = CreateWidget<UUWBP_ResultStats>(GetWorld(), WBP_ResultStats))
    {
        Stats->AddToViewport(100);
        CurrentWidget = Stats;

        Stats->SetStats(KillCount, ElapsedTime);

        if (Stats->Btn_Restart)
        {
            Stats->Btn_Restart->OnClicked.Clear();
            Stats->Btn_Restart->OnClicked.AddDynamic(this, &AMainGameMode::OnRestartClicked);
            Stats->Btn_Restart->SetClickMethod(EButtonClickMethod::PreciseClick);
        }
        if (Stats->Btn_Return)
        {
            Stats->Btn_Return->OnClicked.Clear();
            Stats->Btn_Return->OnClicked.AddDynamic(this, &AMainGameMode::OnReturnMenuClicked);
            Stats->Btn_Return->SetClickMethod(EButtonClickMethod::PreciseClick);
        }

        if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
        {
            PC->bShowMouseCursor      = true;
            PC->bEnableClickEvents    = true;
            PC->bEnableMouseOverEvents= true;

            FInputModeUIOnly Mode;
            if (Stats->Btn_Return)
                Mode.SetWidgetToFocus(Stats->Btn_Return->TakeWidget());
            else
                Mode.SetWidgetToFocus(Stats->TakeWidget());
            Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            PC->SetInputMode(Mode);

            FlushAllPressedKeys(PC);
        }

        UGameplayStatics::SetGamePaused(this, true);
    }
}

void AMainGameMode::OnRestartClicked()
{
    UGameplayStatics::SetGamePaused(this, false);

    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        PC->bShowMouseCursor      = false;
        PC->bEnableClickEvents    = false;
        PC->bEnableMouseOverEvents= false;

        FInputModeGameOnly Mode;
        PC->SetInputMode(Mode);
    }

    if (CurrentWidget)
    {
        CurrentWidget->RemoveFromParent();
        CurrentWidget = nullptr;
    }

    UGameplayStatics::OpenLevel(this, FName(TEXT("TestMap")));
}

void AMainGameMode::OnReturnMenuClicked()
{
    UGameplayStatics::SetGamePaused(this, false);

    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        PC->bShowMouseCursor      = false;
        PC->bEnableClickEvents    = false;
        PC->bEnableMouseOverEvents= false;

        FInputModeGameOnly Mode;
        PC->SetInputMode(Mode);
    }

    if (CurrentWidget)
    {
        CurrentWidget->RemoveFromParent();
        CurrentWidget = nullptr;
    }

    UGameplayStatics::OpenLevel(this, FName(TEXT("MenuLevel")));
}
void AMainGameMode::SetWeaponIconUI(UTexture2D* Icon)
{
    if (HUDWidget) { HUDWidget->SetWeaponIcon(Icon); }
}
void AMainGameMode::UpdateAmmoUI(int32 Current, int32 Reserve, bool bInfinite)
{
    if (HUDWidget) { HUDWidget->UpdateAmmoText(Current, Reserve, bInfinite); }
}


void AMainGameMode::ShowHelp()
{
    if (!HelpWidget && WBP_Help)                 // <- 하드코딩 경로 대신 에디터에서 지정한 클래스 사용
    {
        if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
        {
            HelpWidget = CreateWidget<UUserWidget>(PC, WBP_Help);
            if (HelpWidget)
            {
                HelpWidget->AddToViewport();
                FInputModeUIOnly M; M.SetWidgetToFocus(HelpWidget->TakeWidget());
                M.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
                PC->SetInputMode(M);
                PC->bShowMouseCursor = true;
            }
        }
    }
}
void AMainGameMode::OnHelpButtonClicked()
{
    ShowHelp();
}

void AMainGameMode::HideHelp()
{
    if (HelpWidget)
    {
        HelpWidget->RemoveFromParent();
        HelpWidget = nullptr;

        APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
        if (PC && CurrentWidget)
        {
            FInputModeUIOnly InputMode;
            InputMode.SetWidgetToFocus(CurrentWidget->TakeWidget());
            InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            PC->SetInputMode(InputMode);
            PC->bShowMouseCursor = true;
        }
    }
}
