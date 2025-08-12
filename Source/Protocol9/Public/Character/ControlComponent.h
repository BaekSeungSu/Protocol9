#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ControlComponent.generated.h"
struct FInputActionValue;
class AMainCharacter;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCoolTimeSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLastSkillSignature);

class AMainCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROTOCOL9_API UControlComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UControlComponent();

	//이벤트
	UPROPERTY(BlueprintAssignable, Category = "Skill")
	FOnCoolTimeSignature OnCoolTime;
	UPROPERTY(BlueprintAssignable, Category = "Skill")
	FLastSkillSignature LastSkillCharge;

protected:
	AMainCharacter* Owner;

	UPROPERTY(EditAnywhere, Category = "Melee")
	FVector MeleeBoxHalfSize = FVector(80.0f, 80.0f, 80.0f);
	UPROPERTY(EditAnywhere, Category = "Melee")
	float MeleeRange = 200.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control")
	float DashPower;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control")
	float MaxSpeed;

	float CurrentPitch;
	
	FTimerHandle DashTimer;
	
	virtual void BeginPlay() override;

public:	

	UFUNCTION()
	void DisableInput();
	UFUNCTION()
	void EnableInput();
	
	UFUNCTION()
	void Move(const FInputActionValue& Value);
	UFUNCTION()
	void Look(const FInputActionValue& Value);
	UFUNCTION()
	void StartFire(const FInputActionValue& Value);
	UFUNCTION()
	void StopFire(const FInputActionValue& Value);
	UFUNCTION()
	void Melee(const FInputActionValue& Value);
	UFUNCTION()
	void MeleeAttack();
	UFUNCTION()
	void Reload(const FInputActionValue& Value);
	UFUNCTION()
	void Dash(const FInputActionValue& Value);
	UFUNCTION()
	void StartJump(const FInputActionValue& Value);
	UFUNCTION()
	void StopJump(const FInputActionValue& Value);
	UFUNCTION()
	void SwapWeapon1(const FInputActionValue& Value);
	UFUNCTION()
	void SwapWeapon2(const FInputActionValue& Value);
	UFUNCTION()
	void DeBug1(const FInputActionValue& Value);
	UFUNCTION()
	void DeBug2(const FInputActionValue& Value);


	UFUNCTION(BlueprintCallable,Category="Speed")
	void SetMaxSpeed(float NewMaxSpeed);
	UFUNCTION(BlueprintPure,Category="Speed")
	float GetMaxSpeed() const {return MaxSpeed;};
	UFUNCTION()
	void AddSpeed(float Multiplier);
	//������ ���ǵ� ���� ���� �Լ�
	UFUNCTION()
	void ResetSpeed();
	
	void DoNothing (){};

	UFUNCTION()
	void HandleCharacterDeath();

private:
	bool bInputEnabled = true;

	void BeginSwapWeapon(int32 SlotIndex);
	void FinishSwapWeapon(int32 SlotIndex);
};
