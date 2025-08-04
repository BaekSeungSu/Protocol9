// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ControlComponent.generated.h"
struct FInputActionValue;
class AMainCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROTOCOL9_API UControlComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UControlComponent();

private:
	bool bInputEnabled = true;
	
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
	void Fire(const FInputActionValue& Value);
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
	void AddSpeed(float Multiplier);
	UFUNCTION()
	void ResetSpeed();
	
	void DoNothing (){};

	void HandleCharacterDeath();
};
