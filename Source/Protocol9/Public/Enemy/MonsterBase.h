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
class AMainCharacter;
UCLASS()
class PROTOCOL9_API AMonsterBase : public ACharacter
{
    GENERATED_BODY()

public:
    AMonsterBase();

protected:
    virtual void BeginPlay() override;

public:
    UFUNCTION(BlueprintCallable)
    float GetMonsterHalfHeight() const;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EXP")
    int32 Exp = 100;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    EMonsterState CurrentState;
    
    UFUNCTION()
    void DeleteMonster();
    
    UPROPERTY(BlueprintAssignable)
    FOnMonsterDead OnMonsterDead;
    
    UPROPERTY(BlueprintAssignable)
    FOnMonsterDeadLocation OnMonsterDeadLocation;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
    float WalkSpeed = 600.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ExtraDistance")
    float ExtraDistance;
protected:
   

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Attack")
    float AttackRange = 150.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Attack")
    float AttackCooldown = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Attack")
    class UAnimMontage* AttackMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
    float RunSpeed = 600.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shape")
    float MonsterHalfHeight = 88.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shape")
    float MonsterRadius = 34.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HP")
    float MaxHP;



    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float AIUpdateInterval = 0.1f;
    virtual void ChasePlayer();
    void SetState(EMonsterState NewState);
    bool SetAnimInstance();

    virtual void PossessedBy(AController* NewController) override;
    virtual bool IsInAttackRange(float ExtraDistanceInside) const;
    
    UFUNCTION()
    virtual void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
    // UFUNCTION()
    // void AttackingBehavior();
    
    UFUNCTION()
    virtual void OnDeath();
    UFUNCTION()
    void Ragdoll();
    
    UFUNCTION()
    FVector FindGroundLocation() const;
    UFUNCTION()
    void GiveExp() const;
    UFUNCTION()
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
    UPROPERTY()
    TWeakObjectPtr<APawn> TargetPlayer;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat", meta = (AllowPrivateAccess = "true"))
    bool bShouldContinueAttacking = false;
    UFUNCTION()
    void StopContinuousAttack();
    UFUNCTION()
    void StopMovement();
    virtual void MoveToTarget();
    virtual FVector GetTargetMonsterLocation() const;
    UPROPERTY()
    class AAIController* AIController;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
    class USoundBase* AttackSound;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HP")
    float CurrentHP;
    UFUNCTION()
    virtual void PerformAttack();
  
    
private:
    void UpdateAI();
    
    FVector StartLocation;
   
    bool FindAndSetTargetPlayer();
    bool SetTargetPlayer(APawn* NewTarget);
    
    UFUNCTION()
    void StartContinuousAttack();

    UFUNCTION()
    void StartAIUpdateTimer(const float Jitter);
    UFUNCTION()
    void EndDeath();
    UPROPERTY()
    FVector TargetLocationBefore;
    UFUNCTION()
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    FTimerHandle AIUpdateTimerHandle;
    FTimerHandle DeadTimerHandle;
};