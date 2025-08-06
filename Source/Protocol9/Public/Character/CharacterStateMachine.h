#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterStateMachine.generated.h"

class AMainCharacter;

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	Idle	UMETA(DisplayName = "Idle"),
	Fire	UMETA(DisplayName = "Fire"),
	Melee	UMETA(DisplayName = "Melee"),
	Reload	UMETA(DisplayName = "Reload"),
	Dead	UMETA(DisplayName = "Dead"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStateChanged,
	ECharacterState,
	OldState,
	ECharacterState,
	NewState
	);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROTOCOL9_API UCharacterStateMachine : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCharacterStateMachine();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	ECharacterState CurrentState = ECharacterState::Idle;

	float AnimationStartTime = 0.0f;
	float CurrentAnimationDuration  = 0.0f;

	UPROPERTY()
	AMainCharacter* Owner;
	
public:

	UPROPERTY(BlueprintAssignable, Category = "State")
	FOnStateChanged OnStateChanged;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UAnimMontage* DeathMontage;
	
	UFUNCTION()
	void HandleCharacterDeath();
	
	bool CanReload() const{return CurrentState == ECharacterState::Reload
		|| CurrentState == ECharacterState::Dead;}

	bool CanFire() const{return CurrentState == ECharacterState::Reload
		|| CurrentState == ECharacterState::Dead
		|| CurrentState == ECharacterState::Melee;}

	bool CanMelee() const{return CurrentState == ECharacterState::Dead
		|| CurrentState == ECharacterState::Melee;}

	bool CanSwapWeapon() const{return CurrentState != ECharacterState::Dead;}
	
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
	
	void SetState(ECharacterState NewState);
	void ResetState();
private:
	
	void StopCurrentMontage();
};




