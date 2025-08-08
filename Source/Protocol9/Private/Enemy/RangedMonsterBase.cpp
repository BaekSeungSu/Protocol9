// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/RangedMonsterBase.h"
#include "Enemy/MonsterProjectile.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"

ARangedMonsterBase::ARangedMonsterBase()
{
	AttackRange = 1000.0f; 
	AttackCooldown = 3.0f; 
}

void ARangedMonsterBase::PerformAttack()
{
	if (!bShouldContinueAttacking || !TargetPlayer || !IsInAttackRange())
	{
		StopContinuousAttack();
		return;
	}
	StopMovement();
	if (AttackMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(AttackMontage);
	}
    
	FVector Direction = (TargetPlayer->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	FRotator LookRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
	SetActorRotation(LookRotation);

	if (ProjectileClass)
	{
		FVector SpawnLocation = GetProjectileSpawnLocation();
		FVector TargetLocation = TargetPlayer->GetActorLocation();
        
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
        
		if (AMonsterProjectile* Projectile = GetWorld()->SpawnActor<AMonsterProjectile>(
			ProjectileClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams))
		{
			Projectile->FireAtTarget(TargetLocation, ProjectileArcHeight);
		}

	}
    
	if (AttackMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(AttackMontage);
	}
}

void ARangedMonsterBase::ChasePlayer()
{
	if (!TargetPlayer || !AIController) return;

	if (IsInAttackRange() && HasLineOfSightToPlayer())
	{
		SetState(EMonsterState::Attacking);
		return;
	}
	MoveToTarget();
}

bool ARangedMonsterBase::HasLineOfSightToPlayer()
{
	if (!TargetPlayer)
		return false;
	FVector PlayerLocation = TargetPlayer->GetActorLocation();
	FVector MonsterLocation = GetActorLocation();
	FHitResult HitResult;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	bool bHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), MonsterLocation, PlayerLocation, UEngineTypes::ConvertToTraceType(ECC_Visibility),false, ActorsToIgnore,EDrawDebugTrace::None, HitResult, true);
	return !bHit || (bHit && HitResult.GetActor() == TargetPlayer);
}

FVector ARangedMonsterBase::GetProjectileSpawnLocation()
{
	FVector ForwardVector = GetActorForwardVector();
	FVector RightVector = GetActorRightVector();
	FVector UpVector = GetActorUpVector();
    
	return GetActorLocation() + 
		   (ForwardVector * ProjectileSpawnOffset.X) +
		   (RightVector * ProjectileSpawnOffset.Y) +
		   (UpVector * ProjectileSpawnOffset.Z);
}

