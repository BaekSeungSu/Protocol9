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
#include "NavigationSystem.h"
#include "GameFramework/PlayerController.h"
#include "Components/CapsuleComponent.h"
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
	}
	if (GetMesh())
	{
		GetMesh()->SetAnimInstanceClass(UMonsterBaseAnimInstance::StaticClass());
	}

	CurrentState = EMonsterState::Idle;
	TargetPlayer = nullptr;
	LastAttackTime = 0.0f;
}

void AMonsterBase::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();

	SetAnimInstance();
	SetState(EMonsterState::Chasing);

	AIUpdateInterval = FMath::RandRange(AIUpdateInterval - AIUpdateInterval * 0.2f,
	                                    AIUpdateInterval + AIUpdateInterval * 0.2f);
	StartAIUpdateTimer();
	FindAndSetTargetPlayer();
	float RandomLifeTime = FMath::RandRange(5.0f, 10.0f);
	GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, this, &AMonsterBase::OnDeath, RandomLifeTime, false);
}

void AMonsterBase::UpdateAI()
{
	switch (CurrentState)
	{
	case EMonsterState::Dead:
		break;
	case EMonsterState::Idle:

	case EMonsterState::Chasing:
		ChasePlayer();
		break;

	case EMonsterState::Attacking:
		AttackingBehavior();
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
		GetCharacterMovement()->MaxWalkSpeed = 0.0f;
		if (AIController)
		{
			AIController->StopMovement();
		}
		break;
	}
}

float AMonsterBase::GetMonsterHalfHeight() const
{
	return MonsterHalfHeight;
}

void AMonsterBase::ChasePlayer()
{
	if (!TargetPlayer || !AIController) return;

	if (IsInAttackRange())
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
	if (NewTarget && NewTarget->IsA<APawn>())
	{
		TargetPlayer = NewTarget;
		return true;
	}
	return false;
}

void AMonsterBase::AttackPlayer()
{
	if (!TargetPlayer) return;


	if (bShouldContinueAttacking) return;

	StartContinuousAttack();
}

void AMonsterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AIController = Cast<AAIController>(GetController());
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

void AMonsterBase::MoveToTarget()
{
	FVector TargetLocation = GetTargetLocation();
	AIController->MoveToLocation(TargetLocation);
}

FVector AMonsterBase::GetTargetLocation() const
{
	if (TargetPlayer)
	{
		FVector TargetLocation = TargetPlayer->GetActorLocation();

		UNavigationSystemV1* NavSys = UNavigationSystemV1::GetNavigationSystem(GetWorld());
		FNavLocation NavLocation;
		const FVector QueryExtent = FVector(100, 100, 300);
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

void AMonsterBase::EndDeath()
{
	Destroy();
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
			SetState(EMonsterState::Chasing);
		}
	}
}

void AMonsterBase::AttackingBehavior()
{
	if (!TargetPlayer)
	{
		SetState(EMonsterState::Chasing);
		return;
	}

	if (!IsInAttackRange())
	{
		if (bShouldContinueAttacking)
		{
			StopContinuousAttack();
		}

		SetState(EMonsterState::Chasing);
		return;
	}

	if (!bShouldContinueAttacking && IsInAttackRange())
	{
		StartContinuousAttack();
	}
}

void AMonsterBase::OnDeath()
{
	if (CurrentState == EMonsterState::Dead)
	{
		return;
	}

	SetState(EMonsterState::Dead);

	Ragdoll();	

	GiveExp();
	
	DropItems();
	
	OnMonsterDead.Broadcast(this);
	OnMonsterDeadLocation.Broadcast(FindGroundLocation());
	GetWorldTimerManager().SetTimer(DeadTimerHandle,this,&AMonsterBase::EndDeath, 5.0f, false);
}

void AMonsterBase::Ragdoll()
{
	if (AIController)
	{
		AIController->StopMovement();
	}
	
	GetWorld()->GetTimerManager().ClearTimer(AIUpdateTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();
	
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetAnimInstanceClass(nullptr);

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
}

void AMonsterBase::ClearMonster()
{
	//스포너에 보스 스폰 델리게이트에 연결해서 보스가 스폰 되는순간 멀티 델리게이트에 구독해놓은 모든 몬스터의 clearmonster가 호출되며 경험치나 아이템 드롭
	//사망 몽타주 실행 없이 즉시 레벨에서 삭제
	GetWorld()->GetTimerManager().ClearTimer(AIUpdateTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
	Destroy();
}


void AMonsterBase::DropItems() const
{
	UWorld* World = GetWorld();
	
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
		UE_LOG(LogTemp, Warning, TEXT("Hitted Bone: %s"), *PointEvent->HitInfo.BoneName.ToString());
	}
	//찾아보니 TakeDamage는 하나만 있어도 ApplyPointDamage와 ApplyRadialDamage를 둘 다 대응 가능하고 DamageEvent를 상속받는 다른 두가지 DamageEvent를 자동으로 받아서 사용하면
	//된다길래 하나만 만들었습니다. DamageType과 맞은 부위 뼈 이름을 출력하게 만들어 놓았으니 일단 이걸로 테스트 해보시면 될 것 같습니다.
	CurrentHP -= ActualDamage;
	CurrentHP = FMath::Clamp(CurrentHP, 0.0f, MaxHP);
	if (CurrentHP == 0.0f)
	{
		OnDeath();
	}
	return ActualDamage;
}
