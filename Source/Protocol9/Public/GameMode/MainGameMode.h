#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UI/UWBP_GameOver.h"
#include "MainGameMode.generated.h"

class UUWBP_HUD;
class UUserWidget;
class AMonsterSpawner;
class AMonsterBase;
class UWBP_GameOver;

UCLASS()
class PROTOCOL9_API AMainGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UUWBP_HUD* GetHUDWidget() const { return HUDWidget; }


	// 위젯 클래스 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> WBP_MainMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUWBP_HUD> WBP_HUD;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI", meta=(AllowPrivateAccess="true"))
	TSubclassOf<class UWBP_GameOver> WBP_GameOver;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
	TSubclassOf<class UUserWidget> WBP_PressAnyKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
	TSubclassOf<class UUserWidget> WBP_ResultStats;

	// UI 전환 함수
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowMainMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowHUD();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowGameOver(bool bVictory, int32 InKillCount);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void StartGame();
	
	UFUNCTION(BlueprintCallable, Category="UI")
	void OnAnyKeyPressed();
	
	void ShowPressAnyKey();

	UFUNCTION()
	void OnRetryClicked();

	UFUNCTION()
	void ShowResultStats();
	UFUNCTION()
	void OnRestartClicked();
	
	UFUNCTION()
	void OnReturnMenuClicked();

	// 타이머 갱신
	void UpdateGameTimer();
	
	UFUNCTION(BlueprintCallable)
	void NotifyCharacterHUDReady();

	UFUNCTION(BlueprintCallable)
	void AddKillCount();

	UFUNCTION()
	void HandlePlayerDeath();
	
	UFUNCTION()
	void HandleBossDefeated();

	UFUNCTION()
	void PostShowGameOver();


private:
	UPROPERTY()
	UUserWidget* CurrentWidget;

	UPROPERTY()
	UUWBP_HUD* HUDWidget;
	
	
	FTimerHandle GameTimerHandle;
	FTimerHandle PauseTimerHandle;
	float ElapsedTime = 0.0f;

	// === Kill UI 전용 멤버 ===
	UPROPERTY() int32 KillCount = 0;
	bool bKillBindingInitialized = false;
	
	void BindSpawnerForKillCount();

	// 
	UFUNCTION() void OnMonsterSpawnedForUI(AMonsterBase* Monster);
	UFUNCTION() void OnMonsterDeadForUI(AMonsterBase* Monster);
};
