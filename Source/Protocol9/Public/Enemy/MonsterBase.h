#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "MonsterBase.generated.h"

UENUM(BlueprintType)
enum class EMonsterState : uint8
{
    Idle        UMETA(DisplayName = "Idle"),
    Patrolling  UMETA(DisplayName = "Patrolling"),
    Chasing     UMETA(DisplayName = "Chasing"),
    Attacking   UMETA(DisplayName = "Attacking"),
    Returning   UMETA(DisplayName = "Returning")
};

class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
class UAIPerceptionComponent;

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
    UAIPerceptionComponent* AIPerceptionComponent;
    UPROPERTY()
    UAISenseConfig_Sight* SightConfig;
    UPROPERTY()
    UAISenseConfig_Hearing* HearingConfig;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    EMonsterState CurrentState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Perception")
    float SightRadius = 2500.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Perception")
    float HearingRadius = 1500.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Perception")
    float PeripheralVisionAngleDegrees = 180.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Patrol")
    float PatrolRadius = 800.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Patrol")
    float WaitTimeAtPatrolPoint = 3.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Chase")
    float ChaseRadius = 2000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Chase")
    float LosePlayerTime = 5.0f;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Attack")
    float AttackRange = 150.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Attack")
    float AttackCooldown = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Attack")
    class UAnimMontage* AttackMontage;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
    float WalkSpeed = 200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
    float RunSpeed = 400.0f;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shape")
    float MonsterHalfHeight = 88.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shape")
    float MonsterRadius = 34.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HP")
    int32 MaxHP = 100;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EXP")
    int32 Exp = 100;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float AIUpdateInterval = 0.1f;
    //UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropItems") //아이템 베이스 클래스 만들어지면 주석 해제
    //TArray<TSubclassOf<class AItemBase>> DropItems;
protected:

    void UpdateAI();
    void SetState(EMonsterState NewState);
    

    void StartPatrol();
    void Patrol();
    FVector GetRandomPatrolPoint();
    

    void ChasePlayer();
    bool CanSeePlayer();
    

    void AttackPlayer();
    bool IsInAttackRange();
    

    void ReturnToStart();

    UFUNCTION()
    void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
    

    UFUNCTION()
    void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
    UFUNCTION()
    void AttackingBehavior();
    
    UFUNCTION()
    virtual void OnDeath();
    UFUNCTION()
    void DropItems();
    UFUNCTION()
    void GiveExp();
    UFUNCTION()
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
    UPROPERTY()
    APawn* TargetPlayer;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat", meta = (AllowPrivateAccess = "true"))
    bool bShouldContinueAttacking = false;
    UFUNCTION()
    void StopContinuousAttack();
private:   
    FVector StartLocation;
    FVector CurrentPatrolTarget;
    
    float LastAttackTime;
    float PlayerLostTime;
    float PatrolWaitTimer;
    
    UPROPERTY()
    class AAIController* AIController;
    
    bool bIsMovingToPatrolPoint;
    bool bIsWaitingAtPatrolPoint;
    bool bHasReachedPatrolPoint;

    INT32 CurrentHP;

private:
    FTimerHandle AttackTimerHandle;
   
    UFUNCTION()
    virtual void PerformAttack();
    
    UFUNCTION()
    void StartContinuousAttack();
    


    UFUNCTION()
    void StartAIUpdateTimer();

    UFUNCTION()
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    FTimerHandle AIUpdateTimerHandle;
    
};