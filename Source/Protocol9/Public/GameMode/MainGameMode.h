#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainGameMode.generated.h"

class UUWBP_HUD;
class UUserWidget;

UCLASS()
class PROTOCOL9_API AMainGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
	UUWBP_HUD* GetHUDWidget() const { return HUDWidget; }


	// 위젯 클래스 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> WBP_MainMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUWBP_HUD> WBP_HUD;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> WBP_GameOver;

	// UI 전환 함수
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowMainMenu(bool bIsRestart);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowHUD();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowGameOver();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void StartGame();

	// 타이머 갱신
	void UpdateGameTimer();
	
	UFUNCTION(BlueprintCallable)
	void NotifyCharacterHUDReady();

private:
	UPROPERTY()
	UUserWidget* CurrentWidget;

	UPROPERTY()
	UUWBP_HUD* HUDWidget;

	FTimerHandle GameTimerHandle;
	float ElapsedTime = 0.0f;
};
