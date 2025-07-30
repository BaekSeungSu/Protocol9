// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StaminaComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROTOCOL9_API UStaminaComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UStaminaComponent();

protected:

	class ACharacter* Owner;
	
	int MaxStaminaCount;
	int CurrentStaminaCount;
	float StaminaChargeTime;
	
	FTimerHandle StaminaChargeTimer;
	
	virtual void BeginPlay() override;

public:

	int GetMaxStaminaCount() const;
	int GetCurrentStaminaCount() const;
	float GetStaminaChargeTime() const;

	void SetMaxStaminaCount(int NewMaxStaminaCount);
	void SetCurrentStaminaCount(int NewCurremStaminaCount);
	void SetStaminaChargeTime(float NewStaminaChargeTime);
	
	void UseStamina();

	void ChargeStamina();
};
