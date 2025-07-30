// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ControlComponent.generated.h"
class AMainCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROTOCOL9_API UControlComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UControlComponent();

protected:
	AMainCharacter* Owner;

	float DashPower;

	int MaxJumpCount = 2;
	int JumpCount = 0;

	
	FTimerHandle DashTimer;
	
	virtual void BeginPlay() override;

public:	
	
	UFUNCTION()
	void Move(const FInputActionValue& Value);
	UFUNCTION()
	void Look(const FInputActionValue& Value);
	UFUNCTION()
	void Fire(const FInputActionValue& Value);
	UFUNCTION()
	void Dash(const FInputActionValue& Value);
	UFUNCTION()
	void StartJump(const FInputActionValue& Value);
	UFUNCTION()
	void StopJump(const FInputActionValue& Value);

	void DoNothing (){};
};
