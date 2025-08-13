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
	Swapping UMETA(DisplayName = "Swapping")
};

UENUM(BlueprintType)
enum class EHPState : uint8
{
	LowHealth		UMETA(DisplayName = "LowHealth"),
	NormalHealth	UMETA(DisplayName = "NormalHealth"),
	Dead		UMETA(DisplayName = "Dead"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLowHealthSignature);

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

	//이벤트
	UPROPERTY(BlueprintAssignable, Category = "State")
	FOnStateChanged OnStateChanged;
	UPROPERTY(BlueprintAssignable, Category = "State")
	FLowHealthSignature LowHealthEvent;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	ECharacterState CurrentState = ECharacterState::Idle;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	EHPState CurrentHPState = EHPState::NormalHealth;

	float AnimationStartTime = 0.0f;
	float CurrentAnimationDuration  = 0.0f;

	UPROPERTY()
	AMainCharacter* Owner;
	
public:

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UAnimMontage* DeathMontage;
	
	UFUNCTION()
	void HandleCharacterDeath();
	
	bool CanPerformAction() const{
		return CurrentState == ECharacterState::Idle && CurrentHPState != EHPState::Dead;
	}

	bool CanPerformFire() const{
		return CurrentState == ECharacterState::Idle
		&& CurrentHPState != EHPState::Dead
		&& CurrentState != ECharacterState::Swapping
		&& CurrentState != ECharacterState::Reload
		&& CurrentState != ECharacterState::Melee;
	}

	bool CanPerformMelee() const{
		return CurrentState == ECharacterState::Idle
		&& CurrentHPState != EHPState::Dead
		&& CurrentState != ECharacterState::Melee;
	}

	bool CanPerformReload() const{
		return CurrentState == ECharacterState::Idle
		&& CurrentHPState != EHPState::Dead
		&& CurrentState != ECharacterState::Melee
		&& CurrentState != ECharacterState::Reload
		&& CurrentState != ECharacterState::Swapping;
	}
	
	bool CanFire() const { return CanPerformAction();}
	bool CanReload() const {return CanPerformAction();}
	bool CanMelee() const {return CanPerformMelee();}
	bool CanSwapping() const {return CanPerformAction();}

	
	
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
	ECharacterState GetCurrentState() const { return CurrentState; }
	void SetState(ECharacterState NewState);
	void ResetState();
	
	void SetHPState(EHPState NewHPState);
	void ResetHPState();
	
private:
	
	void StopCurrentMontage();
};




