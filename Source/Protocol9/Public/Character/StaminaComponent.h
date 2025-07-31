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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	int MaxStaminaCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	int CurrentStaminaCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaChargeTime;
	
	FTimerHandle StaminaChargeTimer;
	
	virtual void BeginPlay() override;

public:

	UFUNCTION(BlueprintPure, Category = "Stamina")
	int GetMaxStaminaCount() const;
	UFUNCTION(BlueprintPure, Category = "Stamina")
	int GetCurrentStaminaCount() const;
	UFUNCTION(BlueprintPure, Category = "Stamina")
	float GetStaminaChargeTime() const;

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void SetMaxStaminaCount(int NewMaxStaminaCount);
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void SetCurrentStaminaCount(int SetCurrentStaminaCount);
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void SetStaminaChargeTime(float NewStaminaChargeTime);

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void UseStamina();
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void ChargeStamina();
};
