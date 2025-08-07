
#include "Character/SoundComponent.h"
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

	AudioComponent = Owner->FindComponentByClass<UAudioComponent>();

	if (Owner)
	{
		Owner->GetHPComponent()->OnDeathEvent.AddDynamic(this, &USoundComponent::PlayDeathSound);
		Owner->LevelUPEvent.AddDynamic(this, &USoundComponent::PlayLevelUpSound);
	}
}

void USoundComponent::PlayMeleeSound()
{
	AudioComponent->SetSound(MeleeCue);
	AudioComponent->Play();
}

void USoundComponent::PlayJumpSound()
{
	AudioComponent->SetSound(JumpCue);
	AudioComponent->Play();
}

void USoundComponent::PlayDeathSound()
{
	if (AudioComponent->IsPlaying())
	{
		AudioComponent->Stop();
	}
	AudioComponent->SetSound(DeathCue);
	AudioComponent->Play();
}

void USoundComponent::PlayLevelUpSound(int CharacterLevel)
{
	if (AudioComponent->IsPlaying())
	{
		AudioComponent->Stop();
	}
	AudioComponent->SetSound(LevelUpCue);
	AudioComponent->Play();
}

