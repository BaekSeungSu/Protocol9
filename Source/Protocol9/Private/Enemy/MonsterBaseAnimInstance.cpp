#include "Enemy/MonsterBaseAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"

UMonsterBaseAnimInstance::UMonsterBaseAnimInstance()
{
    MonsterOwner = nullptr;
    Speed = 0.0f;
    bIsMoving = false;
    bIsWalking = false;
    bIsRunning = false;
    bIsAttacking = false;
    MonsterState = EMonsterState::Idle;
    
    MovingThreshold = 3.0f;
    WalkingSpeedMax = 250.0f;
    RunningSpeedMin = 250.0f;
    
    PreviousSpeed = 0.0f;
    bWasMovingLastFrame = false;
}

void UMonsterBaseAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    APawn* OwnerPawn = TryGetPawnOwner();
    if (OwnerPawn)
    {
        MonsterOwner = Cast<AMonsterBase>(OwnerPawn);
        if (!MonsterOwner)
        {
            UE_LOG(LogTemp, Warning, TEXT("AnimInstance: Failed to cast pawn to MonsterBase"));
        }
    }
    AnimationStartOffset = FMath::RandRange(0.0f, 1.0f);
}

void UMonsterBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (!MonsterOwner)
    {
        APawn* OwnerPawn = TryGetPawnOwner();
        if (OwnerPawn)
        {
            MonsterOwner = Cast<AMonsterBase>(OwnerPawn);
        }
        return;
    }

    UpdateMovementProperties(DeltaSeconds);
    
    UpdateCombatProperties();
    
    MonsterState = MonsterOwner->CurrentState;
}

void UMonsterBaseAnimInstance::UpdateMovementProperties(float DeltaSeconds)
{
    UCharacterMovementComponent* MovementComp = MonsterOwner->GetCharacterMovement();
    if (!MovementComp) return;
    
    FVector Velocity = MonsterOwner->GetVelocity();
    Speed = Velocity.Size();
    
    bWasMovingLastFrame = bIsMoving;
    bIsMoving = Speed > MovingThreshold;
    
    float CurrentMaxSpeed = MovementComp->MaxWalkSpeed;
    
    if (bIsMoving)
    {
        if (CurrentMaxSpeed <= WalkingSpeedMax)
        {
            bIsWalking = true;
            bIsRunning = false;
        }
        else if (CurrentMaxSpeed >= RunningSpeedMin)
        {
            bIsWalking = false;
            bIsRunning = true;
        }
        else
        {
            bIsWalking = Speed <= WalkingSpeedMax;
            bIsRunning = Speed > RunningSpeedMin;
        }
    }
    else
    {
        bIsWalking = false;
        bIsRunning = false;
    }

    PreviousSpeed = Speed;
}

void UMonsterBaseAnimInstance::UpdateCombatProperties()
{
    UAnimInstance* AnimInstance = MonsterOwner->GetMesh()->GetAnimInstance();
    if (AnimInstance)
    {
        bIsAttacking = AnimInstance->IsAnyMontagePlaying();
        
        // if (MonsterOwner->AttackMontage)
        // {
        //     bIsAttacking = AnimInstance->Montage_IsPlaying(MonsterOwner->AttackMontage);
        // }
    }
}

