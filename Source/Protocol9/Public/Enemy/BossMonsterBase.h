#pragma once

#include "CoreMinimal.h"
#include "Enemy/MonsterBase.h"
#include "BossAttackPatternDataRow.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "BossMonsterBase.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossDeath);
UCLASS(BlueprintType)
class PROTOCOL9_API ABossMonsterBase : public AMonsterBase
{
	GENERATED_BODY()
public:
	ABossMonsterBase();
	
	UFUNCTION()
	void OnPhaseChangeMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	UPROPERTY()
	float DamageModifier;
protected:
	virtual void BeginPlay() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void PerformAttack() override;
	virtual void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted) override;
	bool IsAttackRelatedMontage(UAnimMontage* Montage);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|Phase1")
	TObjectPtr<UDataTable> AttackPatternTable;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|Phase1")
	TObjectPtr<UAnimMontage> BossPhaseChangeAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|Phase2")
	TObjectPtr<UAnimMontage> BossLandingAnimMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase")
	TObjectPtr<UMaterialInterface> OverlayMaterial;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pattern|Cooldown")
	float SkillCooldown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase2|Modifier")
	float Phase2SpeedModifier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase2|Modifier")
	float Phase2DefenceModifier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase2|Modifier")
	float Phase2DamageModifier;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pattern|Cooldown")
	float Pattern1StartTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pattern|Cooldown")
	float Pattern2StartTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pattern|Cooldown")
	float Pattern3StartTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|Range")
	float Pattern3ExtraRange;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pattern|Range")
	float BaseExtraDistance;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Phase")
	EBossPhase CurrentPhase;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Phase")
	bool bCanUsePattern1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Phase")
	bool bCanUsePattern2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Phase")
	bool bCanUsePattern3;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Phase")
	bool bIsPhaseChanging;

	virtual void OnDeath() override;
	FOnBossDeath OnBossDeath;
private:
	bool bHasEnteredPhase2;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DynamicOverlayMaterialInstance;

	FTimerHandle Pattern1TimerHandle;
	FTimerHandle Pattern2TimerHandle;
	FTimerHandle Pattern3TimerHandle;
	FTimerHandle PhaseChangeTimerHandle;
	FTimerHandle OverlayMaterialTimerHandle;
	FTimerHandle LandingTimerHandle;
	void StartPhase2Transition();
	void StartOverlayEffect();
	void UpdateOverlayEffect();
	void FinishPhaseTransition();

	void ExecuteAttackPattern(EBossAttackPattern PatternType);
	UFUNCTION()
	void OnPattern1Ready();
	UFUNCTION()
	void OnPattern2Ready();
	UFUNCTION()
	void OnPattern3Ready();
	UFUNCTION()
	void OnPhaseChangeCompleted();
	
	float CurrentOverlayValue;
	UFUNCTION()
	void DelayedDestory();
	UPROPERTY()
	float DefenceModifier;
	FTimerHandle DeathTimer;
};
