// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"
#include "UI/UWBP_HUD.h"
#include "UI/PlayerUIComponent.h"
#include "MainCharacter.generated.h"

class AMonsterBase;
struct FInputActionValue;
class USpringArmComponent;
class UCameraComponent;
class UHPComponent;
class UStaminaComponent;
class UControlComponent;
class UInventoryComponent;
class UPlayerUIComponent;
class UAudioComponent;
class USoundComponent;
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

	//이벤트
	UPROPERTY(BlueprintAssignable, Category = "Exp")
	FExpChangedSignature ExpChanged;
	UPROPERTY(BlueprintAssignable, Category = "Exp")
	FLevelUPSignature LevelUPEvent;
	
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
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Exp")
	int CharacterLevel;

	//죽었을때 사용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	FName DeathCameraSocket = TEXT("head");
	
	FVector OriginalSpringArmLocation;
	FRotator OriginalSpringArmRotation;
	USceneComponent* OriginalSpringArmParent;

	//컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	UAudioComponent* SituationAudioComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	UAudioComponent* DialogueAudioComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HP")
	UHPComponent* HPComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stamina")
	UStaminaComponent* StaminaComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Control")
	UControlComponent* ControlComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UPlayerUIComponent* PlayerUIComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sound")
	USoundComponent* SoundComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	UInventoryComponent* InventoryComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StateMachine")
	UCharacterStateMachine* StateMachine;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AWeaponBase> DefaultWeaponClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUWBP_HUD> HUDWidgetClass;


	void EquipDefaultWeapon();
	
	virtual void BeginPlay() override;

	//캐릭터 정보 초기화
	void InitCharacterInfo();
	
	// HUD 참조 저장
	UPROPERTY()
	UUWBP_HUD* CachedHUD;

	// 각 효과 리셋용 타이머 핸들
	FTimerHandle InvincibilityResetHandle;
	FTimerHandle SpeedBoostResetHandle;
	FTimerHandle AttackBoostResetHandle;


public:
	

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UStaminaComponent* GetStaminaComponent() const { return StaminaComponent; }
	UControlComponent* GetControlComponent() const { return ControlComponent; }
	UHPComponent* GetHPComponent() const { return HPComponent; }
	USoundComponent* GetSoundComponent() const { return SoundComponent; }
	UInventoryComponent* GetInventoryComponent() const {return InventoryComponent; }
	UCharacterStateMachine* GetStateMachine() const { return StateMachine;}
	UAudioComponent* GetSituationAudioComponent() const { return SituationAudioComponent; }
	UAudioComponent* GetDialogueAudioComponent() const { return DialogueAudioComponent; }
	
	UFUNCTION()
	void AddAttack(float Multiplied);
	UFUNCTION()
	void ResetAttack();

	//무기관련 애님몽타주는 무기로 옮길예정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UAnimMontage* FireMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UAnimMontage* ReloadMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UAnimMontage* OnHandMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UAnimMontage* MeleeMontage;

	virtual float TakeDamage(
		float DamageAmount, 
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

	//몬스터 Exp 핸들러
	UFUNCTION()
	void SetMonsterDead(AMonsterBase* Monster);
	UFUNCTION()
	void OnMonsterDead(AMonsterBase* Monster);
	
	void LevelUp();
	
	// HUD 효과 제어 함수
	void HandleInvincibilityEffect();
	void HandleSpeedBoostEffect();
	void HandleAttackBoostEffect();

	UFUNCTION()
	void CacheHUD();

};