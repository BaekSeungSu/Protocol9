// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/FlyingMonsterBase.h"
#include "Enemy/MonsterProjectile.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"

AFlyingMonsterBase::AFlyingMonsterBase()
{
	AttackRange = 200.0f;
	AttackCooldown = 5.0f;
	
}

void AFlyingMonsterBase::BeginPlay()
{
	Super::BeginPlay();
	InitializeMovement();
}
void AFlyingMonsterBase::InitializeMovement()
{
	// Flying 모드로 설정
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	GetCharacterMovement()->MaxFlySpeed = WalkSpeed;
	GetCharacterMovement()->bOrientRotationToMovement = true;
    
	// 중력 영향 받지 않음
	GetCharacterMovement()->GravityScale = 0.0f;
}
FVector AFlyingMonsterBase::GetTargetMonsterLocation() const
{
	if (TargetPlayer)
	{
		FVector PlayerLocation = TargetPlayer->GetActorLocation();
		UE_LOG(LogTemp, Log, TEXT("PlayerLocation : %s"), *PlayerLocation.ToString());
		PlayerLocation.Z += FlyingHeight; // 플레이어 위 일정 높이
		return PlayerLocation;
	}
	UE_LOG(LogTemp, Log, TEXT("TargetPlayer is NULL"));
	return GetActorLocation();
}

void AFlyingMonsterBase::PerformAttack()
{
	Super::PerformAttack();
	
	if (AttackMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(AttackMontage);
	}
}

void AFlyingMonsterBase::SpawnProjectile()
{
	if (ProjectileClass)
	{
		FVector SpawnLocation = GetProjectileSpawnLocation();
		FVector TargetLocation = TargetPlayer->GetActorLocation();
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
        
		AMonsterProjectile* Projectile = GetWorld()->SpawnActor<AMonsterProjectile>(
			ProjectileClass,
			SpawnLocation,
			FRotator::ZeroRotator,
			SpawnParams);
		Projectile->FireAtTarget(TargetLocation, 0.0f);
	}
}

bool AFlyingMonsterBase::IsInAttackRange(float ExtraDistanceInside) const
{
	if (!TargetPlayer) return false;
	FVector MonsterLocation = GetActorLocation();
	FVector PlayerLocation = TargetPlayer->GetActorLocation();
	
	float DeltaX = PlayerLocation.X - MonsterLocation.X;
	float DeltaY = PlayerLocation.Y - MonsterLocation.Y;
	float DistanceSquared = DeltaX * DeltaX + DeltaY * DeltaY;
	float AttackRangeSquared = AttackRange * AttackRange;
    
	return DistanceSquared <= AttackRangeSquared + ExtraDistance;
}

void AFlyingMonsterBase::MoveToTarget()
{
	FVector CurrentLocation = GetActorLocation();
	FVector TargetLocation = GetTargetMonsterLocation();
	FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal();
	
    if (!Direction.IsNearlyZero())
    {
    	FRotator TargetRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
    	SetActorRotation(TargetRotation);
    }
	// Velocity 직접 설정
	GetCharacterMovement()->Velocity = Direction * RunSpeed;
}

FVector AFlyingMonsterBase::GetProjectileSpawnLocation() const
{
	FVector ForwardVector = GetActorForwardVector();
	FVector RightVector = GetActorRightVector();
	FVector UpVector = GetActorUpVector();
    
	return GetActorLocation() + 
		   (ForwardVector * ProjectileSpawnOffset.X) +
		   (RightVector * ProjectileSpawnOffset.Y) +
		   (UpVector * ProjectileSpawnOffset.Z);
}
