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

private:
	int MaxStaminaCount;
	int CurrentStaminaCount;
	float StaminaChargeTime;
	
protected:
	
	FTimerHandle StaminaChargeTimer;
	
	virtual void BeginPlay() override;

public:	

	void UseStamina();

	void ChargeStamina();
};
