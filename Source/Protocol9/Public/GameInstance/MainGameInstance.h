#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"
#include "MainGameInstance.generated.h"

UCLASS()
class PROTOCOL9_API UMainGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// ===== 결과 데이터 =====
	UPROPERTY(BlueprintReadWrite) int32 LastKillCount = 0;
	UPROPERTY(BlueprintReadWrite) float LastElapsedTime = 0.f;
	UPROPERTY(BlueprintReadWrite) bool  bVictory = false;
	UPROPERTY(BlueprintReadWrite) bool  bPendingResults = false;

	UFUNCTION(BlueprintCallable)
	void SetFinalStats(int32 InKill, float InTime, bool bInVictory)
	{
		LastKillCount   = InKill;
		LastElapsedTime = InTime;
		bVictory        = bInVictory;
		bPendingResults = true;
	}

	// lifecycle
	virtual void Init() override;
	virtual void Shutdown() override;

	// ===== BGM API =====
	UFUNCTION(BlueprintCallable, Category="BGM") void PlayMenuBGM(float Fade = 0.8f);
	UFUNCTION(BlueprintCallable, Category="BGM") void PlayStageBGM(float Fade = 0.8f);
	UFUNCTION(BlueprintCallable, Category="BGM") void PlayBossBGM(float Fade = 0.5f);
	UFUNCTION(BlueprintCallable, Category="BGM") void PlayGameOverBGM(bool bIsVictory, float Fade = 0.5f);
	UFUNCTION(BlueprintCallable, Category="BGM") void StopBGM(float FadeOut = 0.5f);
	bool PlayBGMInternal(USoundBase* Cue, float FadeInTime, float Volume = 1.f);
private:
	bool EnsureMusicComp();
	
	
	// 월드와 분리된, 영속적인 오디오 컴포넌트
	UPROPERTY(Transient)
	UAudioComponent* MusicComp = nullptr;

	// 같은 곡일 때 재시작/깜빡임 방지
	UPROPERTY(Transient) USoundBase* CurrentCue = nullptr;
	void RegisterAudioToWorld(UWorld* World);
	void UnregisterAudioFromWorld(UWorld* World);

	// ★ 맵 로드/월드 정리 델리게이트
	void OnPreLoadMap(const FString& MapName);
	void OnPostLoadMap(UWorld* LoadedWorld);
	void OnWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources);

public: // 에디터에서 지정
	UPROPERTY(EditDefaultsOnly, Category="BGM") USoundBase* BGM_Menu    = nullptr; // PressAnyKey, MainMenu, Help 공통
	UPROPERTY(EditDefaultsOnly, Category="BGM") USoundBase* BGM_Stage   = nullptr; // WBP_HUD(일반)
	UPROPERTY(EditDefaultsOnly, Category="BGM") USoundBase* BGM_Boss    = nullptr; // 보스
	UPROPERTY(EditDefaultsOnly, Category="BGM") USoundBase* BGM_Victory = nullptr; // 승리
	UPROPERTY(EditDefaultsOnly, Category="BGM") USoundBase* BGM_Defeat  = nullptr; // 패배
	UPROPERTY(BlueprintReadWrite) USoundBase* LastPlayedBGM = nullptr;
};
