#include "GameInstance/MainGameInstance.h"

void UMainGameInstance::Init()
{
	Super::Init();

	EnsureMusicComp();
	
	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UMainGameInstance::OnPreLoadMap);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UMainGameInstance::OnPostLoadMap);
	FWorldDelegates::OnWorldCleanup.AddUObject(this, &UMainGameInstance::OnWorldCleanup);
	
	if (UWorld* World = GetWorld())
	{
		RegisterAudioToWorld(World);
	}
}

void UMainGameInstance::Shutdown()
{
	if (MusicComp)
	{
		if (MusicComp->IsPlaying())
			MusicComp->Stop();

		if (UWorld* CompWorld = MusicComp->GetWorld())
		{
			UnregisterAudioFromWorld(CompWorld);
		}

		MusicComp->DestroyComponent();
		MusicComp = nullptr;
	}
	
	FCoreUObjectDelegates::PreLoadMap.RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
	FWorldDelegates::OnWorldCleanup.RemoveAll(this);

	Super::Shutdown();
}

bool UMainGameInstance::EnsureMusicComp()
{
	UWorld* World = GetWorld();
	if (!World || World->bIsTearingDown)         
		return false;

	if (!IsValid(MusicComp))
	{
		MusicComp = NewObject<UAudioComponent>(this); 
		MusicComp->bAutoActivate        = false;
		MusicComp->bIsUISound           = true;
		MusicComp->bAllowSpatialization = false;
		MusicComp->bAutoDestroy         = false;
	}
	
	if (!MusicComp->IsRegistered() || MusicComp->GetWorld() != World)
	{
		if (MusicComp->IsRegistered())
			MusicComp->UnregisterComponent();

		MusicComp->RegisterComponentWithWorld(World);
	}
	
	if (!IsValid(MusicComp) || !MusicComp->IsRegistered())
		return false;

	return true;
}
void UMainGameInstance::RegisterAudioToWorld(UWorld* World)
{
	if (MusicComp && World && !MusicComp->IsRegistered())
	{
		MusicComp->RegisterComponentWithWorld(World);
	}
}

void UMainGameInstance::UnregisterAudioFromWorld(UWorld* World)
{
	if (MusicComp && MusicComp->IsRegistered() && MusicComp->GetWorld() == World)
	{
		MusicComp->Stop();
		MusicComp->UnregisterComponent();
	}
}

void UMainGameInstance::OnPreLoadMap(const FString& /*MapName*/)
{
	if (UWorld* World = GetWorld())
	{
		// 맵 교체 직전에 등록 해제
		UnregisterAudioFromWorld(World);
	}
}

void UMainGameInstance::OnPostLoadMap(UWorld* LoadedWorld)
{
	// 새 월드에 재등록
	RegisterAudioToWorld(LoadedWorld);
}

void UMainGameInstance::OnWorldCleanup(UWorld* World, bool /*bSessionEnded*/, bool /*bCleanupResources*/)
{
	// 청소 대상 월드에 붙어 있으면 반드시 해제
	UnregisterAudioFromWorld(World);
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
