// Fill out your copyright notice in the Description page of Project Settings.
#include "Enemy/MonsterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Enemy/MonsterBaseAIController.h"
#include "Enemy/MonsterBaseAnimInstance.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/PlayerController.h"
#include "Components/CapsuleComponent.h"

AMonsterBase::AMonsterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	AIControllerClass = AMonsterBaseAIController::StaticClass();
	
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	//SetPerceptionComponent(AIPerceptionComponent);

	if (GetMesh())
	{
		GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -MonsterHalfHeight));
		GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	}


	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCapsuleHalfHeight(MonsterHalfHeight);
		GetCapsuleComponent()->SetCapsuleRadius(MonsterRadius);
	}
	if (GetMesh())
	{
		GetMesh()->SetAnimInstanceClass(UMonsterBaseAnimInstance::StaticClass());
	}


	CurrentState = EMonsterState::Idle;
	TargetPlayer = nullptr;
	LastAttackTime = 0.0f;
	PlayerLostTime = 0.0f;
	PatrolWaitTimer = 0.0f;
	bIsMovingToPatrolPoint = false;
	bIsWaitingAtPatrolPoint = false;
	bHasReachedPatrolPoint = false;
}

void AMonsterBase::BeginPlay()
{
	Super::BeginPlay();


	AIController = Cast<AAIController>(GetController());
	//AMonsterBaseAIController* MonsterAI = Cast<AMonsterBaseAIController>(GetController());

	StartLocation = GetActorLocation();

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &AMonsterBase::OnAttackMontageEnded);
	}
	

	SetState(EMonsterState::Patrolling);

	AIUpdateInterval = FMath::RandRange(AIUpdateInterval-AIUpdateInterval*0.2f, AIUpdateInterval+AIUpdateInterval*0.2f);
	StartAIUpdateTimer();
}

// void AMonsterBase::Tick(float DeltaTime)
// {
// 	Super::Tick(DeltaTime);
// 	UpdateAI();
// }

void AMonsterBase::UpdateAI()
{
	switch (CurrentState)
	{
	case EMonsterState::Idle:

		SetState(EMonsterState::Patrolling);
		break;

	case EMonsterState::Patrolling:
		Patrol();
		break;

	case EMonsterState::Chasing:
		ChasePlayer();
		break;

	case EMonsterState::Attacking:
		AttackingBehavior();
		break;

	case EMonsterState::Returning:
		ReturnToStart();
		break;
	}
}

void AMonsterBase::SetState(EMonsterState NewState)
{
	if (CurrentState == EMonsterState::Attacking && NewState != EMonsterState::Attacking)
	{
		StopContinuousAttack();
	}
	if (CurrentState == NewState) return;

	CurrentState = NewState;

	switch (NewState)
	{
	case EMonsterState::Idle:
		GetCharacterMovement()->MaxWalkSpeed = 0.0f;
		if (AIController)
		{
			AIController->StopMovement();
		}
		break;
	case EMonsterState::Patrolling:
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		StartPatrol();
		break;

	case EMonsterState::Chasing:
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
		PlayerLostTime = 0.0f;
		break;

	case EMonsterState::Attacking:
		GetCharacterMovement()->MaxWalkSpeed = 0.0f;
		if (AIController)
		{
			AIController->StopMovement();
		}
		break;

	case EMonsterState::Returning:
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		if (PlayerLostTime >= LosePlayerTime)
		{
			TargetPlayer = nullptr;
		}
		break;
	}
}

bool AMonsterBase::SetTargetPlayer(APawn* NewTarget)
{
	TargetPlayer = NewTarget;
	if (TargetPlayer && TargetPlayer->IsA<APawn>())
	{
		return true;
	}
	return false;
}

void AMonsterBase::StartPatrol()
{
	CurrentPatrolTarget = GetRandomPatrolPoint();
	bIsMovingToPatrolPoint = true;
	bIsWaitingAtPatrolPoint = false;
	bHasReachedPatrolPoint = false;

	if (AIController)
	{
		AIController->MoveToLocation(CurrentPatrolTarget);
	}
}

void AMonsterBase::Patrol()
{
	if (!AIController) return;


	float DistanceToTarget = FVector::Dist(GetActorLocation(), CurrentPatrolTarget);

	if (bIsMovingToPatrolPoint && DistanceToTarget < 100.0f)
	{
		bIsMovingToPatrolPoint = false;
		bIsWaitingAtPatrolPoint = true;
		bHasReachedPatrolPoint = true;
		PatrolWaitTimer = 0.0f;


		AIController->StopMovement();
	}


	if (bIsWaitingAtPatrolPoint)
	{
		PatrolWaitTimer += AIUpdateInterval;

		if (PatrolWaitTimer >= WaitTimeAtPatrolPoint)
		{
			StartPatrol();
		}
	}
}

FVector AMonsterBase::GetRandomPatrolPoint() const
{

	FVector RandomDirection = FMath::VRand();
	RandomDirection.Z = 0;
	RandomDirection.Normalize();

	float RandomDistance = FMath::RandRange(PatrolRadius * 0.3f, PatrolRadius);
	FVector PatrolPoint = StartLocation + RandomDirection * RandomDistance;

	return PatrolPoint;
}

void AMonsterBase::ChasePlayer()
{
	if (!TargetPlayer || !AIController) return;


	float DistanceToPlayer = FVector::Dist(GetActorLocation(), TargetPlayer->GetActorLocation());

	if (DistanceToPlayer > ChaseRadius || !CanSeePlayer())
	{
		PlayerLostTime += AIUpdateInterval;

		if (PlayerLostTime >= LosePlayerTime)
		{
			SetState(EMonsterState::Returning);
			return;
		}
	}
	else
	{
		PlayerLostTime = 0.0f;
	}


	if (IsInAttackRange())
	{
		SetState(EMonsterState::Attacking);
		return;
	}
	if (DistanceToPlayer <= ChaseRadius)
	{
		AIController->MoveToActor(TargetPlayer);
	}
}

bool AMonsterBase::CanSeePlayer() const
{
	if (!TargetPlayer) return false;


	FVector Start = GetActorLocation();
	FVector End = TargetPlayer->GetActorLocation();

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECollisionChannel::ECC_Visibility,
		QueryParams
	);

	return !bHit || HitResult.GetActor() == TargetPlayer;
}

void AMonsterBase::AttackPlayer()
{
	if (!TargetPlayer) return;


	if (bShouldContinueAttacking) return;

	StartContinuousAttack();
}

void AMonsterBase::StartContinuousAttack()
{
	if (!TargetPlayer) return;

	bShouldContinueAttacking = true;


	PerformAttack();


	GetWorld()->GetTimerManager().SetTimer(
		AttackTimerHandle,
		this,
		&AMonsterBase::PerformAttack,
		AttackCooldown, 
		true
	);
}

void AMonsterBase::PerformAttack()
{

	if (!bShouldContinueAttacking || !TargetPlayer || !IsInAttackRange())
	{
		StopContinuousAttack();
		return;
	}
	


	FVector Direction = (TargetPlayer->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	FRotator LookRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
	SetActorRotation(LookRotation);

	
}

void AMonsterBase::StopContinuousAttack()
{
	

	bShouldContinueAttacking = false;


	if (AttackTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
		AttackTimerHandle.Invalidate();
	}
}

void AMonsterBase::StartAIUpdateTimer()
{
	GetWorld()->GetTimerManager().SetTimer(
		AIUpdateTimerHandle,
		this,
		&AMonsterBase::UpdateAI,
		AIUpdateInterval,
		true 
	);
}

void AMonsterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AIUpdateTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(AIUpdateTimerHandle);
		AIUpdateTimerHandle.Invalidate();
	}
	Super::EndPlay(EndPlayReason);
}

bool AMonsterBase::IsInAttackRange() const
{
	if (!TargetPlayer) return false;

	float DistanceToPlayer = FVector::Dist(GetActorLocation(), TargetPlayer->GetActorLocation());
	return DistanceToPlayer <= AttackRange;
}

void AMonsterBase::ReturnToStart()
{
	if (!AIController) return;

	float DistanceToStart = FVector::Dist(GetActorLocation(), StartLocation);

	if (DistanceToStart < 100.0f)
	{
		SetState(EMonsterState::Patrolling);
		return;
	}

	AIController->MoveToLocation(StartLocation);
}



// void AMonsterBase::OnSeePawn(APawn* Pawn)
// {
//
// 	if (Pawn && Pawn->IsA<APawn>() && Pawn->GetController() && Pawn->GetController()->IsA<APlayerController>())
// 	{
// 		TargetPlayer = Pawn;
//
// 		if (CurrentState != EMonsterState::Attacking)
// 		{
// 			SetState(EMonsterState::Chasing);
// 		}
// 	}
// }
//
// void AMonsterBase::OnHearNoise(APawn* PawnInstigator, const FVector& Location, float Volume)
// {
//
// 	UE_LOG(LogTemp, Log, TEXT("Heard noise at %s"), *Location.ToString());
// 	if (PawnInstigator && PawnInstigator->GetController() && PawnInstigator->GetController()->IsA<APlayerController>())
// 	{
// 		if (CurrentState == EMonsterState::Patrolling || CurrentState == EMonsterState::Idle)
// 		{
// 			TargetPlayer = PawnInstigator;
// 			SetState(EMonsterState::Chasing);
// 		}
// 	}
// }

void AMonsterBase::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == AttackMontage && CurrentState == EMonsterState::Attacking)
	{

		if (TargetPlayer && IsInAttackRange())
		{
			SetState(EMonsterState::Chasing);
		}
		else
		{
			SetState(EMonsterState::Patrolling);
		}
	}
}

void AMonsterBase::AttackingBehavior()
{

	if (!TargetPlayer)
	{
		SetState(EMonsterState::Patrolling);
		return;
	}

	float DistanceToPlayer = FVector::Dist(GetActorLocation(), TargetPlayer->GetActorLocation());

	if (!IsInAttackRange())
	{

		if (bShouldContinueAttacking)
		{
			StopContinuousAttack();
		}


		if (DistanceToPlayer <= ChaseRadius && CanSeePlayer())
		{
			SetState(EMonsterState::Chasing);
			return;
		}
		else
		{
			SetState(EMonsterState::Returning);
			return;
		}
	}

	if (!bShouldContinueAttacking && IsInAttackRange())
	{
		StartContinuousAttack();
		return;
	}


	if (DistanceToPlayer > ChaseRadius || !CanSeePlayer())
	{
		PlayerLostTime += GetWorld()->GetDeltaSeconds();

		if (PlayerLostTime >= LosePlayerTime)
		{
			StopContinuousAttack();
			SetState(EMonsterState::Returning);
		}
	}
	else
	{
		PlayerLostTime = 0.0f;
	}
}

void AMonsterBase::OnDeath()
{
	GiveExp();
	DropItems();
	Destroy();
}

void AMonsterBase::DropItems() const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	//World->SpawnActor<AActor>(AItemBase::StaticClass(), GetActorLocation(), FRotator::ZeroRotator);
}

void AMonsterBase::GiveExp() const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC)
	{
		return;
	}
	APawn* Pawn = PC->GetPawn();
	if (!Pawn)
	{
		return;
	}
	ACharacter* Character = Cast<ACharacter>(Pawn);
	if (!Character)
	{
		return;
	}
	//APlayerState* PlayerCharacterState = Character->GetPlayerState<APlayerState>();
	//if (!PlayerCharacterState)
	//{
	return;
	//}
	//PlayerState->GiveExp(EXP);
}

float AMonsterBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                               class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	CurrentHP -= ActualDamage;
	CurrentHP = FMath::Clamp(CurrentHP, 0.0f, MaxHP);
	if (CurrentHP <= 0.0f)
	{
		OnDeath();
	}
	
	return ActualDamage;
}
