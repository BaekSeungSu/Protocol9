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
	
	bool CanReload() const{return CurrentState == ECharacterState::Reload
		|| CurrentHPState == EHPState::Dead;}

	bool CanFire() const{return CurrentState == ECharacterState::Reload
		|| CurrentHPState == EHPState::Dead
		|| CurrentState == ECharacterState::Melee;}

	bool CanMelee() const{return CurrentHPState == EHPState::Dead
		|| CurrentState == ECharacterState::Melee;}

	bool CanSwapWeapon() const
	{
		const bool bIsActionFree = (CurrentState == ECharacterState::Idle);
		const bool bIsAlive = (CurrentHPState != EHPState::Dead);
		return bIsActionFree && bIsAlive;
	}
	
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
	
	void SetState(ECharacterState NewState);
	void ResetState();
	
	void SetHPState(EHPState NewHPState);
	void ResetHPState();
	
private:
	
	void StopCurrentMontage();
};




