#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MonsterBase.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMonsterDead, AMonsterBase*, DeadMonster);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMonsterDeadLocation, FVector, DeadLocation);
UENUM(BlueprintType)
enum class EMonsterState : uint8
{
    Idle        UMETA(DisplayName = "Idle"),
    Chasing     UMETA(DisplayName = "Chasing"),
    Attacking   UMETA(DisplayName = "Attacking"),
    Dead        UMETA(DisplayName = "Dead")
};

UCLASS()
class PROTOCOL9_API AMonsterBase : public ACharacter
{
    GENERATED_BODY()

public:
    AMonsterBase();

protected:
    virtual void BeginPlay() override;

public:
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    EMonsterState CurrentState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Attack")
    float AttackRange = 150.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Attack")
    float AttackCooldown = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Attack")
    class UAnimMontage* AttackMontage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
    float WalkSpeed = 300.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
    float RunSpeed = 600.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shape")
    float MonsterHalfHeight = 88.0f;

    UFUNCTION(BlueprintCallable)
    float GetMonsterHalfHeight() const;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shape")
    float MonsterRadius = 34.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HP")
    int32 MaxHP = 100;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EXP")
    int32 Exp = 100;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float AIUpdateInterval = 0.1f;
    
    UPROPERTY(BlueprintAssignable)
    FOnMonsterDead OnMonsterDead;
    
    UPROPERTY(BlueprintAssignable)
    FOnMonsterDeadLocation OnMonsterDeadLocation;

protected:
    virtual void ChasePlayer();
    void SetState(EMonsterState NewState);
    bool SetAnimInstance();
    void AttackPlayer();

    virtual void PossessedBy(AController* NewController) override;
    virtual bool IsInAttackRange() const;
    
    UFUNCTION()
    void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
    UFUNCTION()
    void AttackingBehavior();
    
    UFUNCTION()
    virtual void OnDeath();
    UFUNCTION()
    void Ragdoll();
    UFUNCTION()
    void ClearMonster();
    UFUNCTION()
    void DropItems() const;
    UFUNCTION()
    FVector FindGroundLocation() const;
    UFUNCTION()
    void GiveExp() const;
    UFUNCTION()
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
    UPROPERTY()
    APawn* TargetPlayer;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat", meta = (AllowPrivateAccess = "true"))
    bool bShouldContinueAttacking = false;
    UFUNCTION()
    void StopContinuousAttack();
    virtual void MoveToTarget();
    virtual FVector GetTargetMonsterLocation() const;
    UPROPERTY()
    class AAIController* AIController;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
    class USoundBase* AttackSound;
private:
    void UpdateAI();
    
    FVector StartLocation;
    
    float LastAttackTime;
    
    
    int32 CurrentHP;
    
    FTimerHandle AttackTimerHandle;

   
    bool FindAndSetTargetPlayer();
    bool SetTargetPlayer(APawn* NewTarget);
    
    UFUNCTION()
    virtual void PerformAttack();
    
    UFUNCTION()
    void StartContinuousAttack();

    UFUNCTION()
    void StartAIUpdateTimer();
    UFUNCTION()
    void EndDeath();
    UFUNCTION()
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    FTimerHandle AIUpdateTimerHandle;
    FTimerHandle DeadTimerHandle;
};