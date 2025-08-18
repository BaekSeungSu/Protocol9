
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Sound/SoundCue.h"
#include "SoundComponent.generated.h"

class AMainCharacter;
class UAudioComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROTOCOL9_API USoundComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	AMainCharacter* Owner;
	UAudioComponent* Situation;
	UAudioComponent* Dialogue;

public:	
	USoundComponent();

	void PlayMeleeSound();
	void PlayJumpSound();
	UFUNCTION()
	void PlayDeathSound();
	UFUNCTION()
	void PlayLevelUpSound(int CharacterLevel);
	UFUNCTION()
	void PlayOnCoolTimeSound();
	UFUNCTION()
	void PlayLowHealthSound();
	UFUNCTION()
	void PlayBuffedSound();
	UFUNCTION()
	void PlayLastChargeSound();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundCue* MeleeCue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundCue* JumpCue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundCue* DeathCue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundCue* LevelUpCue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundCue* OnCoolTimeCue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundCue* LastChargeCue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundCue* LowHealthCue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundCue* BuffedCue;

	virtual void BeginPlay() override;
	
			
};
