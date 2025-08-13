// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BossMonsterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

ABossMonsterBase::ABossMonsterBase()
{
	CurrentPhase = EBossPhase::Phase1;
	
	bCanUsePattern1 = false;
	bCanUsePattern2 = false;
	bCanUsePattern3 = false;
	
	bIsPhaseChanging = false;
	bHasEnteredPhase2 = false;

	MaxHP = 1000.0f;
	RunSpeed = 800.0f;
	DefenceModifier = 1.0f;
	DamageModifier = 1.0f;
	Pattern3ExtraRange = 2000.0f;
	Phase2DamageModifier = 1.3f;
	Phase2DefenceModifier = 0.8f; // 받는 Damage에 곱해주기 때문에 1보다 작음.
	Phase2SpeedModifier = 1.3f;
	
	CurrentOverlayValue = 0.0f;

	SkillCooldown = 30.0f; // 테스트하려고 바꿈 원래 30.0f
	Pattern1StartTime = 10.0f;
	Pattern2StartTime = 20.0f; //테스트하려고 바꿈 원래 20.0f
	Pattern3StartTime = 30.0f;
}

void ABossMonsterBase::OnPhaseChangeMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == BossLandingAnimMontage)
	{
		SetState(EMonsterState::Chasing);
		OnPhaseChangeCompleted();
	}
}

void ABossMonsterBase::BeginPlay()
{
	Super::BeginPlay();
	if (OverlayMaterial)
	{
		DynamicOverlayMaterialInstance = UMaterialInstanceDynamic::Create(OverlayMaterial, this);
		if (DynamicOverlayMaterialInstance)
		{
			DynamicOverlayMaterialInstance->SetScalarParameterValue(FName("OverlayValue"), CurrentOverlayValue);
		}
	}
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &ABossMonsterBase::OnPhaseChangeMontageEnded);
	}
	GetWorldTimerManager().SetTimer(Pattern1TimerHandle,this,&ABossMonsterBase::OnPattern1Ready, Pattern1StartTime, false);
	GetWorldTimerManager().SetTimer(Pattern2TimerHandle, this, &ABossMonsterBase::OnPattern2Ready, Pattern2StartTime, false);
}

float ABossMonsterBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	
	float ActualDamage = Super::TakeDamage(DamageAmount*DefenceModifier, DamageEvent, EventInstigator, DamageCauser);
	if ((CurrentHP / MaxHP) < 0.5f)
	{
		
		UE_LOG(LogTemp, Warning, TEXT("HP가 50 이하로 떨어졌습니다. Current HP: %f, Max HP: %f"), CurrentHP, MaxHP);
	}
	if (!bHasEnteredPhase2 && !bIsPhaseChanging && (CurrentHP / MaxHP) < 0.5f)
	{
		UE_LOG(LogTemp, Warning, TEXT("HP가 50 이하로 떨어졌습니다. Phase2로 전환합니다."));
		StartPhase2Transition();
	}
	
	return ActualDamage;
}

void ABossMonsterBase::PerformAttack()
{
	Super::PerformAttack();
	if(bIsPhaseChanging)
		return;

	if (bCanUsePattern3)//여기 테스트 하려고 2로 바꿈 원래 3
	{
		ExecuteAttackPattern(EBossAttackPattern::Pattern3); //여기 테스트 하려고 2로 바꿈 원래 3
	}
	else if (bCanUsePattern2)
	{
		ExecuteAttackPattern(EBossAttackPattern::Pattern2);
	}
	else if (bCanUsePattern1)
	{
		ExecuteAttackPattern(EBossAttackPattern::Pattern1);
	}
	else
	{
		if (AttackMontage && GetMesh() && GetMesh()->GetAnimInstance())
		{
			GetMesh()->GetAnimInstance()->Montage_Play(AttackMontage);
		}
		if (AttackSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), AttackSound, GetActorLocation());
		}
	}
}

void ABossMonsterBase::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (IsAttackRelatedMontage(Montage) && CurrentState == EMonsterState::Attacking)
	{
		if (Montage->GetName() == "AM_BossCrunchPattern03")
		{
			UE_LOG(LogTemp, Warning, TEXT("AM_BossCrunchPattern03 Ended"));
			ExtraDistance = BaseExtraDistance;
		}
		if (TargetPlayer && IsInAttackRange(20.0f) && bShouldContinueAttacking)
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

bool ABossMonsterBase::IsAttackRelatedMontage(UAnimMontage* Montage)
{
	if (Montage == AttackMontage)
		return true;

	if (AttackPatternTable)
	{
		TArray<FBossAttackPatternDataRow*> AllRows;
		AttackPatternTable->GetAllRows<FBossAttackPatternDataRow>("",AllRows);

		for (const auto& Row : AllRows)
		{
			UE_LOG(LogTemp, Warning, TEXT("Row->AnimMontage: %s"), *Row->AnimMontage->GetName());
			if (Montage == Row->AnimMontage)
			{
				UE_LOG(LogTemp, Warning, TEXT("Montage = %s"), *Montage->GetName());
				return true;
			}
		}
	}
	return false;
}

void ABossMonsterBase::OnDeath()
{
	if (CurrentState == EMonsterState::Dead)
	{
		return;
	}
	SetState(EMonsterState::Dead);

	OnBossDeath.Broadcast();
	
	GetWorldTimerManager().SetTimer(DeathTimer,this,&ABossMonsterBase::DelayedDestory, 5.0f, false);
}

void ABossMonsterBase::StartPhase2Transition()
{
	DefenceModifier = 0.0f;
	SetState(EMonsterState::Idle);
	bHasEnteredPhase2 = true;
	bIsPhaseChanging = true;
	CurrentPhase = EBossPhase::Phase2;
	
	StopContinuousAttack();
	GetCharacterMovement()->StopMovementImmediately();
	if (GetMesh() && DynamicOverlayMaterialInstance)
	{
		GetMesh()->SetOverlayMaterial(DynamicOverlayMaterialInstance);
	}
	if (BossPhaseChangeAnimMontage)
	{
		float MontageLength = PlayAnimMontage(BossPhaseChangeAnimMontage);
		StartOverlayEffect();
		GetWorldTimerManager().SetTimer(PhaseChangeTimerHandle, this, &ABossMonsterBase::FinishPhaseTransition, MontageLength, false);
	}
}

void ABossMonsterBase::StartOverlayEffect()
{
	CurrentOverlayValue = 0.0f;
	GetWorldTimerManager().SetTimer(OverlayMaterialTimerHandle, this, &ABossMonsterBase::UpdateOverlayEffect, 0.1f, true);
}

void ABossMonsterBase::UpdateOverlayEffect()
{
	CurrentOverlayValue += 10.0f;
	UE_LOG(LogTemp, Warning, TEXT("CurrentOverlayValue: %f"), CurrentOverlayValue);
	if (CurrentOverlayValue >= 300.0f)
	{
		CurrentOverlayValue = 300.0f;
		GetWorldTimerManager().ClearTimer(OverlayMaterialTimerHandle);
	}
	if (DynamicOverlayMaterialInstance)
	{
		DynamicOverlayMaterialInstance->SetScalarParameterValue(FName("OverlayValue"), CurrentOverlayValue);
	}
}

void ABossMonsterBase::FinishPhaseTransition()
{
	if (BossLandingAnimMontage)
	{
		//float MontageLength = PlayAnimMontage(BossLandingAnimMontage);
		PlayAnimMontage(BossLandingAnimMontage); //이건 ACharacter에 만들어져 있는 밑에 함수의 래퍼함수이다. 즉, 밑에거랑 거의 똑같은 기능을 하는데 쉽게 쓸 수 있게 만들어 둔것.
		//GetMesh()->GetAnimInstance()->Montage_Play(BossLandingAnimMontage); // 이건 ACharacter를 상속받지 않은 클래스들이 애님 몽타주를 플레이 할 때 쓴다.
		//GetWorldTimerManager().SetTimer(LandingTimerHandle, this, &ABossMonsterBase::OnPhaseChangeCompleted, MontageLength, false);
	}
}

void ABossMonsterBase::ExecuteAttackPattern(EBossAttackPattern PatternType)
{
	if (!AttackPatternTable)
		return;

	FString PatternName;
	switch (PatternType)
	{
	case EBossAttackPattern::Pattern1:
		PatternName = TEXT("Pattern1");
		break;
	case EBossAttackPattern::Pattern2:
		PatternName = TEXT("Pattern2");
		break;
	case EBossAttackPattern::Pattern3:
		PatternName = TEXT("Pattern3");
		break;
	}

	const FName RowName = FName(*PatternName);
	const FBossAttackPatternDataRow* PatternData = AttackPatternTable->FindRow<FBossAttackPatternDataRow>(RowName,"");

	if (!PatternData || !PatternData->AnimMontage)
		return;

	PlayAnimMontage(PatternData->AnimMontage);
	switch (PatternType)
	{
	case EBossAttackPattern::Pattern1:
		bCanUsePattern1 = false;
		GetWorldTimerManager().SetTimer(Pattern1TimerHandle, this, &ABossMonsterBase::OnPattern1Ready, Pattern1StartTime, false);
		break;
	case EBossAttackPattern::Pattern2:
		bCanUsePattern2 = false;
		GetWorldTimerManager().SetTimer(Pattern2TimerHandle, this, &ABossMonsterBase::OnPattern2Ready, Pattern2StartTime, false);
		break;
	case EBossAttackPattern::Pattern3:
		bCanUsePattern3 = false;
		GetWorldTimerManager().SetTimer(Pattern3TimerHandle, this, &ABossMonsterBase::OnPattern3Ready, Pattern3StartTime, false);
		break;
	}
}

void ABossMonsterBase::OnPattern1Ready()
{
	bCanUsePattern1 = true;
}

void ABossMonsterBase::OnPattern2Ready()
{
	bCanUsePattern2 = true;
}

void ABossMonsterBase::OnPhaseChangeCompleted()
{
	bIsPhaseChanging = false;
	DefenceModifier = 1.0f;
	RunSpeed  *= Phase2SpeedModifier;
	WalkSpeed *= Phase2SpeedModifier;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	DefenceModifier *= Phase2DefenceModifier;
	DamageModifier *= Phase2DamageModifier;

	OnPattern3Ready();
	
	GetWorldTimerManager().ClearTimer(LandingTimerHandle);
	GetWorldTimerManager().ClearTimer(OverlayMaterialTimerHandle);
	GetWorldTimerManager().ClearTimer(PhaseChangeTimerHandle);
	GetWorldTimerManager().ClearTimer(Pattern1TimerHandle);
	GetWorldTimerManager().ClearTimer(Pattern2TimerHandle);
	
	GetWorldTimerManager().SetTimer(Pattern1TimerHandle,this,&ABossMonsterBase::OnPattern1Ready, Pattern1StartTime, true);
	GetWorldTimerManager().SetTimer(Pattern2TimerHandle, this, &ABossMonsterBase::OnPattern2Ready, Pattern2StartTime, true);
	GetWorldTimerManager().SetTimer(Pattern3TimerHandle, this, &ABossMonsterBase::OnPattern3Ready, Pattern3StartTime, true);
	
	SetState(EMonsterState::Chasing);
}

void ABossMonsterBase::DelayedDestory()
{
	Destroy();
}

void ABossMonsterBase::OnPattern3Ready()
{
	if (CurrentPhase == EBossPhase::Phase2)
	{
		bCanUsePattern3 = true;
		BaseExtraDistance = ExtraDistance;
		ExtraDistance = Pattern3ExtraRange;
	}
}
