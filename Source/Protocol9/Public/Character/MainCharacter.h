// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

struct FInputActionValue;
class USpringArmComponent;
class UCameraComponent;
class UHPComponent;
class UStaminaComponent;
class UControlComponent;
class UInventoryComponent;
class UPlayerUIComponent;
class AWeaponBase;
class UCharacterStateMachine;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FExpChangedSignature, int, Exp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLevelUPSignature, int, CharacterLevel);


UCLASS()
class PROTOCOL9_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()
	
public:
	AMainCharacter();
	
protected:

	//이벤트
	UPROPERTY(BlueprintAssignable, Category = "Exp")
	FExpChangedSignature ExpChanged;
	UPROPERTY(BlueprintAssignable, Category = "Exp")
	FLevelUPSignature LevelUPEvent;

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
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Exp")
	int CharacterLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	FName DeathCameraSocket;

	FVector OriginalSpringArmLocation;
	FRotator OriginalSpringArmRotation;
	USceneComponent* OriginalSpringArmParent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HP")
	UHPComponent* HPComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stamina")
	UStaminaComponent* StaminaComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Control")
	UControlComponent* ControlComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UPlayerUIComponent* PlayerUIComponent;

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	UInventoryComponent* InventoryComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AWeaponBase> DefaultWeaponClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StateMachine")
	UCharacterStateMachine* StateMachine;
	

	void EquipDefaultWeapon();
	
	virtual void BeginPlay() override;

public:	

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UStaminaComponent* GetStaminaComponent() const { return StaminaComponent; }
	UControlComponent* GetControlComponent() const { return ControlComponent; }
	UHPComponent* GetHPComponent() const { return HPComponent; }
	
	UInventoryComponent* GetInventoryComponent() const {return InventoryComponent; }
	UCharacterStateMachine* GetStateMachine() const { return StateMachine;}

	UFUNCTION()
	void AddAttack(float Multiplied);
	UFUNCTION()
	void ResetAttack();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UAnimMontage* OnHandMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UAnimMontage* MeleeMontage;

	virtual float TakeDamage(float DamageAmount, 
						struct FDamageEvent const& DamageEvent, 
						class AController* EventInstigator, 
						AActor* DamageCauser) override;
	
	void HideDefalutMesh();
	void ShowDefalutMesh();

	void SetupDeathCamera();
	void ResetCameraToDefault();
	
	int GetAttack() const{return Attack;}
	int GetExp() const {return Exp;}
	int GetCharacterLevel() const {return CharacterLevel;}

	void SetAttack(int NewAttack);
	void SetExp(int Exp);
	void SetLevel(int Level);
	void AddExp(int NewExp);
	
	void LevelUp();
	

};