#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainGameMode.generated.h"

class UUWBP_HUD;
class UUserWidget;
class AMonsterSpawner;
class AMonsterBase;
class UWBP_GameOver;
class UTexture2D;
UCLASS()
class PROTOCOL9_API AMainGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UUWBP_HUD* GetHUDWidget() const { return HUDWidget; }

    // 위젯 클래스
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
    TSubclassOf<UUserWidget> WBP_MainMenu;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
    TSubclassOf<UUWBP_HUD> WBP_HUD;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI", meta=(AllowPrivateAccess="true"))
    TSubclassOf<class UWBP_GameOver> WBP_GameOver;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
    TSubclassOf<class UUserWidget> WBP_PressAnyKey;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
    TSubclassOf<class UUserWidget> WBP_ResultStats;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
    TSubclassOf<UUserWidget> WBP_Help;

    // UI 전환
    UFUNCTION(BlueprintCallable, Category="UI") void ShowMainMenu();
    UFUNCTION(BlueprintCallable, Category="UI") void ShowHUD();
    UFUNCTION(BlueprintCallable, Category="UI") void ShowGameOver(bool bVictory, int32 InKillCount);
    UFUNCTION(BlueprintCallable, Category="UI") void StartGame();
    UFUNCTION(BlueprintCallable, Category="UI") void OnAnyKeyPressed();

    // 결과창/버튼
    UFUNCTION() void OnRetryClicked();
    UFUNCTION() void ShowResultStats();
    UFUNCTION() void OnRestartClicked();
    UFUNCTION() void OnReturnMenuClicked();

    // HUD 연동/타이머/킬수
    void UpdateGameTimer();
    UFUNCTION(BlueprintCallable) void NotifyCharacterHUDReady();
    UFUNCTION(BlueprintCallable) void AddKillCount();

    // 게임오버 트리거
    UFUNCTION() void HandlePlayerDeath();
    UFUNCTION() void HandleBossDefeated();
    UFUNCTION() void PostShowGameOver();

    UFUNCTION()
    void ShowPressAnyKey();

    void SetWeaponIconUI(UTexture2D* Icon);
    void UpdateAmmoUI(int32 Current, int32 Reserve, bool bInfinite);

    void ShowHelp();
    void HideHelp();

    UFUNCTION() void OnHelpButtonClicked();
private:
    UPROPERTY() UUserWidget* CurrentWidget = nullptr;
    UPROPERTY() UUWBP_HUD*   HUDWidget     = nullptr;

    FTimerHandle GameTimerHandle;
    FTimerHandle PauseTimerHandle;
    float ElapsedTime = 0.0f;

    UPROPERTY() int32 KillCount = 0;
    bool bKillBindingInitialized = false;

    void BindSpawnerForKillCount();
    UFUNCTION() void OnMonsterSpawnedForUI(AMonsterBase* Monster);
    UFUNCTION() void OnMonsterDeadForUI(AMonsterBase* Monster);

    UPROPERTY()
    UUserWidget* HelpWidget = nullptr;
};
