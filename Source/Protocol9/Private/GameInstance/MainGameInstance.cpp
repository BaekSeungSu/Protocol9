#include "GameInstance/MainGameInstance.h"

void UMainGameInstance::Init()
{
	Super::Init();
	EnsureMusicComp();
}

void UMainGameInstance::Shutdown()
{
	// 안전 종료
	if (MusicComp)
	{
		if (MusicComp->IsPlaying())
			MusicComp->Stop();

		if (MusicComp->IsRegistered())
			MusicComp->UnregisterComponent();

		MusicComp->DestroyComponent();
		MusicComp = nullptr;
	}
	CurrentCue = nullptr;

	Super::Shutdown();
}

bool UMainGameInstance::EnsureMusicComp()
{
	UWorld* World = GetWorld();
	if (!World || World->bIsTearingDown)          // 월드가 없거나 종료 중이면 작업 금지
		return false;

	if (!IsValid(MusicComp))
	{
		MusicComp = NewObject<UAudioComponent>(this);  // Outer = GameInstance
		MusicComp->bAutoActivate        = false;
		MusicComp->bIsUISound           = true;
		MusicComp->bAllowSpatialization = false;
		MusicComp->bAutoDestroy         = false;
	}

	// 현재 등록된 월드와 다르면 재등록
	if (!MusicComp->IsRegistered() || MusicComp->GetWorld() != World)
	{
		if (MusicComp->IsRegistered())
			MusicComp->UnregisterComponent();

		MusicComp->RegisterComponentWithWorld(World);
	}

	// 방어: 이미 PendingKill 등 비정상 상태면 실패처리
	if (!IsValid(MusicComp) || !MusicComp->IsRegistered())
		return false;

	return true;
}

bool UMainGameInstance::PlayBGMInternal(USoundBase* Cue, float FadeInTime, float Volume)
{
	if (!EnsureMusicComp())                                  // 월드/컴포넌트 준비 안 됐으면 실패
		return false;
	if (!Cue) return false;

	// 같은 곡 재생 중이면 아무것도 안 함 → 끊김 방지
	if (CurrentCue == Cue && MusicComp->IsPlaying())
		return true;

	// 다른 곡이 돌아가면 정리
	if (MusicComp->IsPlaying())
	{
		if (FadeInTime > 0.f) MusicComp->FadeOut(FadeInTime, 0.f);
		else                  MusicComp->Stop();
	}

	CurrentCue = Cue;
	LastPlayedBGM = Cue;
	MusicComp->SetSound(Cue);

	if (FadeInTime > 0.f) MusicComp->FadeIn(FadeInTime, Volume);
	else                  MusicComp->Play(0.f);

	return true;
}

void UMainGameInstance::StopBGM(float FadeOut)
{
	if (!EnsureMusicComp()) return;

	if (MusicComp->IsPlaying())
	{
		if (FadeOut > 0.f) MusicComp->FadeOut(FadeOut, 0.f);
		else               MusicComp->Stop();
	}
}
void UMainGameInstance::PlayMenuBGM(float Fade)               { PlayBGMInternal(BGM_Menu,    Fade); }
void UMainGameInstance::PlayStageBGM(float Fade)              { PlayBGMInternal(BGM_Stage,   Fade); }
void UMainGameInstance::PlayBossBGM(float Fade)               { PlayBGMInternal(BGM_Boss,    Fade); }
void UMainGameInstance::PlayGameOverBGM(bool bIsVictory, float Fade)
{
	PlayBGMInternal(bIsVictory ? BGM_Victory : BGM_Defeat, Fade);
	UE_LOG(LogTemp, Warning, TEXT("PlayGameOverBGM called, Victory: %s"), bIsVictory ? TEXT("true") : TEXT("false"));
}
