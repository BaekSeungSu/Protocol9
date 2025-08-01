// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

struct FInputActionValue;
class UCameraComponent;
class UHPComponent;
class UStaminaComponent;
class UControlComponent;
class UInventoryComponent;
class AWeaponBase;

UCLASS()
class PROTOCOL9_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()
	
public:
	AMainCharacter();

	
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attack")
	float BasetAttack;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attack")
	float LevelUpAttack;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attack")
	float Attack;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attack")
	float CurrentAttack;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Exp")
	int Exp;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Exp")
	int MaxExp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HP")
	UHPComponent* HPComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stamina")
	UStaminaComponent* StaminaComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Control")
	UControlComponent* ControlComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	UInventoryComponent* InventoryComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AWeaponBase> DefaultWeaponClass;


	
	void EquipDefaultWeapon();
	
	virtual void BeginPlay() override;

public:	

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UStaminaComponent* GetStaminaComponent() const { return StaminaComponent; }
	UControlComponent* GetControlComponent() const { return ControlComponent; }
	UHPComponent* GetHPComponent() const { return HPComponent; }
	UFUNCTION()
	void AddAttack(float Multiplied);
	UFUNCTION()
	void ResetAttack();
	UInventoryComponent* GetInventoryComponent() const {return InventoryComponent; }
	

};