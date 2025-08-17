// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StaminaComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCurrentStaminaChangedSignature, int, CurrentStaminaCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStaminaChargeTimeSignature, float, StaminaChargeTime);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROTOCOL9_API UStaminaComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UStaminaComponent();

	//이벤트
	UPROPERTY(BlueprintAssignable, Category = "Stamina")
	FCurrentStaminaChangedSignature StaminaChanged;
	UPROPERTY(BlueprintAssignable, Category = "Stamina")
	FStaminaChargeTimeSignature RemainStaminaChargeTime;
	
protected:
	//이벤트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	int MaxStaminaCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	int CurrentStaminaCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaChargeTime;
	
	FTimerHandle StaminaChargeTimer;
	FTimerHandle SChecktaminaChargeTimer;
	
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
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void CheckStaminaChargeTime();
};
