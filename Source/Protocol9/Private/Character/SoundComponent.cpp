
#include "Character/SoundComponent.h"

#include "Character/CharacterStateMachine.h"
#include "Character/ControlComponent.h"
#include "Character/HPComponent.h"
#include "Character/MainCharacter.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"

USoundComponent::USoundComponent()
{

	PrimaryComponentTick.bCanEverTick = false;

	Owner = Cast<AMainCharacter>(GetOwner());
}


void USoundComponent::BeginPlay()
{
	Super::BeginPlay();

	Situation = Owner->GetSituationAudioComponent();
	Dialogue = Owner->GetDialogueAudioComponent();

	if (Owner)
	{
		Owner->GetHPComponent()->OnDeathEvent.AddDynamic(this, &USoundComponent::PlayDeathSound);
		Owner->LevelUPEvent.AddDynamic(this, &USoundComponent::PlayLevelUpSound);
		Owner->GetStateMachine()->LowHealthEvent.AddDynamic(this, &USoundComponent::PlayLowHealthSound);
		Owner->GetControlComponent()->OnCoolTime.AddDynamic(this, &USoundComponent::PlayOnCoolTimeSound);
		Owner->GetControlComponent()->LastSkillCharge.AddDynamic(this, &USoundComponent::PlayLastChargeSound);
	}
}

void USoundComponent::PlayMeleeSound()
{
	Situation->SetSound(MeleeCue);
	Situation->Play();
}

void USoundComponent::PlayJumpSound()
{
	Situation->SetSound(JumpCue);
	Situation->Play();
}

void USoundComponent::PlayDeathSound()
{
	if (Situation->IsPlaying())
	{
		Situation->Stop();
	}
	Situation->SetSound(DeathCue);
	Situation->Play();
}

void USoundComponent::PlayLevelUpSound(int CharacterLevel)
{
	if (Dialogue->IsPlaying())
	{
		Dialogue->Stop();
	}
	Dialogue->SetSound(LevelUpCue);
	Dialogue->Play();
}

void USoundComponent::PlayOnCoolTimeSound()
{
	if (Dialogue->IsPlaying())
	{
		Dialogue->Stop();
	}
	Dialogue->SetSound(OnCoolTimeCue);
	Dialogue->Play();
}

void USoundComponent::PlayLowHealthSound()
{
	Situation->SetSound(LowHealthCue);
	Situation->Play();
}

void USoundComponent::PlayBuffedSound()
{
}

void USoundComponent::PlayLastChargeSound()
{
	if (Dialogue->IsPlaying())
	{
		Dialogue->Stop();
	}
	Dialogue->SetSound(LastChargeCue);
	Dialogue->Play();
}

