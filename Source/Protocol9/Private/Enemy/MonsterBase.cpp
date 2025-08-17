// Fill out your copyright notice in the Description page of Project Settings.
#include "Enemy/MonsterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Enemy/MonsterBaseAIController.h"
#include "Enemy/MonsterBaseAnimInstance.h"
#include "Engine/World.h"
#include "NavigationSystem.h"
#include "TimerManager.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/PlayerController.h"
#include "Components/CapsuleComponent.h"
#include "Character/MainCharacter.h"
#include "Engine/DamageEvents.h"
AMonsterBase::AMonsterBase()
{
	PrimaryActorTick.bCanEverTick = false;
	AIControllerClass = AMonsterBaseAIController::StaticClass();
	
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);

	if (GetMesh())
	{
		GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -MonsterHalfHeight));
		GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	}

	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCapsuleHalfHeight(MonsterHalfHeight);
		GetCapsuleComponent()->SetCapsuleRadius(MonsterRadius);
		GetCapsuleComponent()->SetCollisionObjectType(ECC_GameTraceChannel3);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECR_Ignore);
	}
	if (GetMesh())
	{
		GetMesh()->SetAnimInstanceClass(UMonsterBaseAnimInstance::StaticClass());
	}

	CurrentState = EMonsterState::Idle;
	TargetPlayer = nullptr;
	ExtraDistance = 20.0f;
}

void AMonsterBase::BeginPlay()
{
	Super::BeginPlay();
	
	StartLocation = GetActorLocation();
	CurrentHP = MaxHP;
	TargetLocationBefore = FVector(FLT_MAX);
	SetAnimInstance();
	SetState(EMonsterState::Chasing);
	
	StartAIUpdateTimer(FMath::RandRange(0.0f,AIUpdateInterval));
	FindAndSetTargetPlayer();
	GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	//GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, this, &AMonsterBase::OnDeath, RandomLifeTime, false);
}

void AMonsterBase::UpdateAI()
{
	switch (CurrentState)
	{
	case EMonsterState::Dead:
		break;
	case EMonsterState::Idle:
		break;
	case EMonsterState::Chasing:
		ChasePlayer();
		break;

	case EMonsterState::Attacking:
		StartContinuousAttack();
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
	case EMonsterState::Dead:
		GetCharacterMovement()->DisableMovement();
		if (AIController)
		{
			AIController->StopMovement();
		}
		break;
	case EMonsterState::Idle:
		GetCharacterMovement()->MaxWalkSpeed = 0.0f;
		if (AIController)
		{
			AIController->StopMovement();
		}
		break;

	case EMonsterState::Chasing:
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
		break;

	case EMonsterState::Attacking:
		//몬스터가 움찔하는 증강이 있어서 이동 멈추는 로직 공격 시점으로 이동
		break;
	}
}

float AMonsterBase::GetMonsterHalfHeight() const
{
	return MonsterHalfHeight;
}

void AMonsterBase::ChasePlayer()
{
	if (!TargetPlayer.IsValid() || !AIController) return;

	if (IsInAttackRange(ExtraDistance))
	{
		SetState(EMonsterState::Attacking);
		return;
	}
	MoveToTarget();
}

bool AMonsterBase::SetAnimInstance()
{
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &AMonsterBase::OnAttackMontageEnded);
	}
	return false;
}

bool AMonsterBase::FindAndSetTargetPlayer()
{
	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PC = World->GetFirstPlayerController())
		{
			if (APawn* PlayerPawn = PC->GetPawn())
			{
				SetTargetPlayer(PlayerPawn);
				return true;
			}
		}
	}
	return false;
}

bool AMonsterBase::SetTargetPlayer(APawn* NewTarget)
{
	AMainCharacter* NewTargetCharacter = Cast<AMainCharacter>(NewTarget);
	if (NewTargetCharacter)
	{
		TargetPlayer = NewTargetCharacter;
		return true;
	}
	return false;
}

void AMonsterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AIController = Cast<AAIController>(GetController());
}

void AMonsterBase::StartContinuousAttack()
{
	if (bShouldContinueAttacking)
	{
		return;
	}
	if (!TargetPlayer.IsValid())
	{
		return;
	}

	bShouldContinueAttacking = true;
	PerformAttack();
	// GetWorld()->GetTimerManager().SetTimer(
	// 	AttackTimerHandle,
	// 	this,
	// 	&AMonsterBase::PerformAttack,
	// 	AttackCooldown,
	// 	true
	// );
}

void AMonsterBase::PerformAttack()
{
	// if (!bShouldContinueAttacking || !TargetPlayer || !IsInAttackRange(20.0f))
	// {
	// 	StopContinuousAttack();
	// 	return;
	// }
	
	StopMovement();
	FVector Direction = (TargetPlayer->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	FRotator LookRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
	SetActorRotation(LookRotation);
}

void AMonsterBase::StopContinuousAttack()
{
	bShouldContinueAttacking = false;
}

void AMonsterBase::StopMovement()
{
	GetCharacterMovement()->MaxWalkSpeed = 0.0f;
	if (AIController)
	{
		AIController->StopMovement();
	}
}

void AMonsterBase::MoveToTarget()
{
	if (!AIController) return;
	
	FVector TargetLocation = TargetPlayer->GetActorLocation();
	if (FVector::DistSquared2D(TargetLocation, TargetLocationBefore) < FMath::Square(100.0f))
	{
		return;
	}
	TargetLocation = GetTargetMonsterLocation();
	TargetLocationBefore = TargetLocation;
	AIController->MoveToLocation(TargetLocation);
}

FVector AMonsterBase::GetTargetMonsterLocation() const
{
	// FVector TargetLocation = TargetPlayer->GetActorLocation();
	// TargetLocation.Z = GetActorLocation().Z;
	// return TargetLocation;
	if (TargetPlayer.IsValid())  // 계단이나 언덕이면 다른 연산이 필요하겠지만 일단 지금은 주석처리
	{
		FVector TargetLocation = TargetPlayer->GetActorLocation();
	
		UNavigationSystemV1* NavSys = UNavigationSystemV1::GetNavigationSystem(GetWorld());
		FNavLocation NavLocation;
		const FVector QueryExtent = FVector(100, 100, 600);
		if (NavSys && NavSys->ProjectPointToNavigation(TargetLocation, NavLocation, QueryExtent))
		{
			return NavLocation.Location;
		}
		UE_LOG(LogTemp, Warning, TEXT("NavSys is NULL"));
		return TargetLocation;
	}
	UE_LOG(LogTemp, Warning, TEXT("TargetPlayer is NULL"));
	return GetActorLocation();
}

void AMonsterBase::StartAIUpdateTimer(const float Jitter)
{
	GetWorld()->GetTimerManager().SetTimer(
		AIUpdateTimerHandle,
		this,
		&AMonsterBase::UpdateAI,
		AIUpdateInterval,
		true,
		Jitter
	);
}

void AMonsterBase::EndDeath()
{
	AIController = nullptr;
	Destroy();
}

void AMonsterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().ClearAllTimersForObject(this);
	}
	
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->OnMontageEnded.RemoveDynamic(this, &AMonsterBase::OnAttackMontageEnded);
	}
	
	if (DeadTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(DeadTimerHandle);
		DeadTimerHandle.Invalidate();
	}
	Super::EndPlay(EndPlayReason);
}

bool AMonsterBase::IsInAttackRange(float ExtraDistanceInside) const
{
	
	if (TargetPlayer.IsValid())
	{
		float DistanceToPlayer = FVector::Dist(GetActorLocation(), TargetPlayer->GetActorLocation());
		return DistanceToPlayer <= AttackRange + ExtraDistanceInside;
	}
	return false;
}

void AMonsterBase::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == AttackMontage && CurrentState == EMonsterState::Attacking)
	{
		if (TargetPlayer.IsValid() && IsInAttackRange(ExtraDistance) && bShouldContinueAttacking)
		{
			SetState(EMonsterState::Attacking);
			PerformAttack();
		}
		else
		{
			bShouldContinueAttacking = false;
			SetState(EMonsterState::Chasing);
		}
	}
}

// void AMonsterBase::AttackingBehavior()
// {
// 	if (!TargetPlayer)
// 	{
// 		SetState(EMonsterState::Chasing);
// 		return;
// 	}
// 	if (!IsInAttackRange(20.0f))
// 	{
// 		if (bShouldContinueAttacking)
// 		{
// 			StopContinuousAttack();
// 		}
// 	
// 		SetState(EMonsterState::Chasing);
// 		return;
// 	}
// 	if (!bShouldContinueAttacking && IsInAttackRange(20.0f))
// 	{
// 		StartContinuousAttack();
// 	}
// }

void AMonsterBase::OnDeath()
{
	if (CurrentState == EMonsterState::Dead)
	{
		return;
	}

	
	SetState(EMonsterState::Dead);

	Ragdoll();	

	GiveExp();

	OnMonsterDead.Broadcast(this);
	OnMonsterDeadLocation.Broadcast(FindGroundLocation());
	GetWorldTimerManager().SetTimer(DeadTimerHandle,this,&AMonsterBase::EndDeath, 1.0f, false);
}

void AMonsterBase::Ragdoll()
{
	if (AIController)
	{
		AIController->StopMovement();
	}
	
	GetWorld()->GetTimerManager().ClearTimer(AIUpdateTimerHandle);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();
	
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetAnimInstanceClass(nullptr);

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
}

void AMonsterBase::DeleteMonster()
{
	//스포너에 보스 스폰 델리게이트에 연결해서 보스가 스폰 되는순간 멀티 델리게이트에 구독해놓은 모든 몬스터의 clearmonster가 호출되며 경험치나 아이템 드롭
	//사망 몽타주 실행 없이 즉시 레벨에서 삭제
	GetWorld()->GetTimerManager().ClearTimer(AIUpdateTimerHandle);
	Destroy();
}



FVector AMonsterBase::FindGroundLocation() const
{
	FVector StartPoint = GetActorLocation();
	FVector EndPoint = StartPoint - FVector(0.f, 0.f, 10000.f); 
    
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartPoint,
		EndPoint,
		ECC_WorldStatic,
		QueryParams
	);

	if (bHit)
	{
		return HitResult.Location;
	}
	
	return GetActorLocation();
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
	//if (PlayerCharacterState)
	//{
	//	PlayerState->GiveExp(EXP); 이건 플레이어 스테이트 만들어진거 보고 맞춰서 작성
	//}
}

float AMonsterBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                               class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	UE_LOG(LogTemp, Warning, TEXT("Damage Type : %s , Damage Amount: %f"), *DamageEvent.DamageTypeClass->GetName(),
	       ActualDamage);
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent* PointEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
		UE_LOG(LogTemp, Warning, TEXT("Hit Bone: %s"), *PointEvent->HitInfo.BoneName.ToString());
		if (*PointEvent->HitInfo.BoneName.ToString() == FName("head") || *PointEvent->HitInfo.BoneName.ToString() == FName("neck_01"))
		{
			UE_LOG(LogTemp, Warning, TEXT("★★★★★★★★★★ Head Shot! ★★★★★★★★★★"));
			ActualDamage *= 2.0f;
		}
	}
	
	CurrentHP -= ActualDamage;
	CurrentHP = FMath::Clamp(CurrentHP, 0.0f, MaxHP);
	if (CurrentHP == 0.0f)
	{
		OnDeath();
	}
	return ActualDamage;
}
