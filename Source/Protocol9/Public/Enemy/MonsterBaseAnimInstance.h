#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MonsterBase.h"
#include "MonsterBaseAnimInstance.generated.h"

UCLASS()
class PROTOCOL9_API UMonsterBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UMonsterBaseAnimInstance();

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void UpdateMovementProperties(float DeltaSeconds);

	UFUNCTION(BlueprintCallable, Category = "Animation") 
	void UpdateCombatProperties();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster")
	class AMonsterBase* MonsterOwner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StateMachine")
	bool bIsMoving;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StateMachine")
	bool bIsWalking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StateMachine")
	bool bIsRunning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StateMachine")
	bool bIsAttacking;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	EMonsterState MonsterState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Thresholds")
	float MovingThreshold = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Thresholds")
	float WalkingSpeedMax = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Thresholds")
	float RunningSpeedMin = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|StartOffset")
	float AnimationStartOffset;
	UFUNCTION()
	void AnimNotify_AttackCheck() const;
private:
	float PreviousSpeed;
	bool bWasMovingLastFrame;
};
