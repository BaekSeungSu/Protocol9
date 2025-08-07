
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SoundComponent.generated.h"

class AMainCharacter;
class UAudioComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROTOCOL9_API USoundComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	AMainCharacter* Owner;
	UAudioComponent* AudioComponent;

public:	
	USoundComponent();

	void PlayMeleeSound();
	void PlayJumpSound();
	UFUNCTION()
	void PlayDeathSound();
	UFUNCTION()
	void PlayLevelUpSound(int CharacterLevel);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundCue* MeleeCue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundCue* JumpCue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundCue* DeathCue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundCue* LevelUpCue;

	virtual void BeginPlay() override;
	
			
};
