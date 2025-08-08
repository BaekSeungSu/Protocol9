#include "Character/ControlComponent.h"
#include "Character/MainCharacter.h"
#include "Character/StaminaComponent.h"
#include "Character/CharacterStateMachine.h"
#include "Character/HPComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "EnhancedInputComponent.h"
#include "Character/SoundComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Weapons/InventoryComponent.h"
#include "Weapons/WeaponBase.h"
#include "Enemy/MonsterBase.h"

#include "Engine/DamageEvents.h"


UControlComponent::UControlComponent()
	:DashPower(1500.0f),
	MaxSpeed(600.0f)
{

	PrimaryComponentTick.bCanEverTick = false;

	Owner = Cast<AMainCharacter>(GetOwner());

}


// Called when the game starts
void UControlComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Owner)
	{
		Owner->GetCharacterMovement()->MaxWalkSpeed = MaxSpeed;

		UHPComponent* HPComp = Owner->GetHPComponent();
		if (HPComp)
		{
			HPComp->OnDeathEvent.AddDynamic(this,&UControlComponent::HandleCharacterDeath);
		}

		EnableInput();
	}
}

void UControlComponent::DisableInput()
{
	bInputEnabled = false;
}

void UControlComponent::EnableInput()
{
	bInputEnabled = true;
}

void UControlComponent::Move(const FInputActionValue& Value)
{
	if (!Owner->Controller) return;

	if (!bInputEnabled) return;
	
	const FVector2D MoveInput = Value.Get<FVector2D>();

	
	
	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		//FVector direction = FRotationMatrix(Owner->GetControlRotation()).GetScaledAxis(EAxis::X);
		FVector direction = Owner->GetActorForwardVector();

		direction = FVector(direction.X, direction.Y, 0.f).GetSafeNormal();
		
		Owner->AddMovementInput(direction, MoveInput.X);
	}
	
	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		Owner->AddMovementInput(Owner->GetActorRightVector(), MoveInput.Y);
	}
}

void UControlComponent::Look(const FInputActionValue& Value)
{
	if (!Owner->Controller) return;

	if (!bInputEnabled) return;
	
	const FVector2D LookInput = Value.Get<FVector2D>();

	Owner->AddControllerYawInput(LookInput.X);

	Owner->AddControllerPitchInput(-LookInput.Y);
}

void UControlComponent::StartFire(const FInputActionValue& Value)
{
	if (!bInputEnabled || !Owner || !Owner->Controller) return;
	if (Owner->GetStateMachine()->CanFire()) return;

	if (Owner->GetInventoryComponent()->GetCurrentWeapon()->GetCurrentAmmo() == 0)
	{
		Reload(Value);
		return;
	}
	
	AWeaponBase* CurrentWeapon = Owner->GetInventoryComponent()->GetCurrentWeapon();
	if (CurrentWeapon && CurrentWeapon->Implements<UWeaponInterface>())
	{
		IWeaponInterface::Execute_PrimaryFire(CurrentWeapon);
	}
	
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Fire"));

	UAnimInstance* AnimInstance = Owner->GetMesh()->GetAnimInstance();
	{
		if (AnimInstance && Owner->FireMontage)
		{
			float duration = AnimInstance->Montage_Play(Owner->FireMontage);
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("%f"), duration));
		}
	}
}

void UControlComponent::StopFire(const FInputActionValue& Value)
{
	if (!bInputEnabled || !Owner || !Owner->Controller) return;
	
	AWeaponBase* CurrentWeapon = Owner->GetInventoryComponent()->GetCurrentWeapon();
	if (CurrentWeapon && CurrentWeapon->Implements<UWeaponInterface>())
	{
		IWeaponInterface::Execute_StopFire(CurrentWeapon);
	}
}

void UControlComponent::Melee(const FInputActionValue& Value)
{
	if (!Owner->Controller) return;

	if (!bInputEnabled) return;
	
	if (Owner->GetStateMachine()->CanMelee()) return;
	
	UAnimInstance* AnimInstance = Owner->GetMesh()->GetAnimInstance();
	{
		if (Owner->GetSoundComponent())
		{
			Owner->GetSoundComponent()->PlayMeleeSound();
		}
		
		if (AnimInstance && Owner->MeleeMontage)
		{
			Owner->GetStateMachine()->SetState(ECharacterState::Melee);
			
			float duration = AnimInstance->Montage_Play(Owner->MeleeMontage);
			
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Melee"));

			FOnMontageEnded MeleeEnded;
			MeleeEnded.BindLambda([this](UAnimMontage* Montage, bool bInterrupted)
			{
				if (Owner && Owner->GetStateMachine())
				{
					Owner->GetStateMachine()->SetState(ECharacterState::Idle);
				}
			});

			AnimInstance->Montage_SetEndDelegate(MeleeEnded, Owner->MeleeMontage);

		}
	}
	
}

void UControlComponent::MeleeAttack()
{
	TArray<AActor*> HitActors;

	FVector Start = Owner->GetActorLocation();
	FVector Forward = Owner->GetActorForwardVector();
	FVector End = Start + (Forward * MeleeRange);

	TArray<FHitResult> HitResults;
	FCollisionShape Box = FCollisionShape::MakeBox(FVector(MeleeBoxHalfSize));
	FRotator BoxRotation = Owner->GetControlRotation();

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());
	
#if ENABLE_DRAW_DEBUG
	DrawDebugBox(GetWorld(), End, MeleeBoxHalfSize, BoxRotation.Quaternion(), FColor::Red, false, 1.0f);
#endif

	bool bHit = GetWorld()->SweepMultiByObjectType(
		HitResults,
		Start,
		End,
		BoxRotation.Quaternion(),
		ObjectQueryParams,
		Box,
		QueryParams
	);

	if (bHit)
	{
		int HitCount = 0;

		for (FHitResult& Hit : HitResults)
		{
			if (AActor* Target = Cast<AActor>(Hit.GetActor()))
			{
				HitActors.Add(Hit.GetActor());

				FVector Direction = (Hit.ImpactPoint - Owner->GetActorLocation()).GetSafeNormal();
				
				if (AMonsterBase* Monster = Cast<AMonsterBase>(Target))
				{
					UGameplayStatics::ApplyDamage(
					Target,
					Owner->GetAttack(),
					Owner->GetController(),
					Owner,
					UDamageType::StaticClass()
					);
					
					Monster->GetMesh()->AddImpulse(Direction * 100.0f);
					HitCount++;

				}
			}
		}
		
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("MeleeAttack Complete"));
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("%d"), HitActors.Num()));

	}

	
}

void UControlComponent::Reload(const FInputActionValue& Value)
{
	if (!Owner->Controller) return;

	if (!bInputEnabled) return;
	
	if (Owner->GetStateMachine()->CanFire()
		/*&& Owner->GetInventoryComponent()->GetCurrentWeapon()->GetCurrentAmmo() == */) return;
		//현재 무기탄약이 가득차있으면 리턴 무기의 max탄약 Get해야함 
	Owner->GetStateMachine()->SetState(ECharacterState::Reload);

	if (Owner->GetInventoryComponent())
	{
		AWeaponBase* CurrentWeapon = Owner->GetInventoryComponent()->GetCurrentWeapon();
		if (CurrentWeapon && CurrentWeapon->Implements<UWeaponInterface>())
		{
			IWeaponInterface::Execute_Reload(CurrentWeapon);
		}
	}

	
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, (TEXT("Reload!")));

	UAnimInstance* AnimInstance = Owner->GetMesh()->GetAnimInstance();
	{
		if (AnimInstance && Owner->ReloadMontage)
		{
			float duration = AnimInstance->Montage_Play(Owner->ReloadMontage);

			FOnMontageEnded ReloadEnded;
			ReloadEnded.BindLambda([this](UAnimMontage* Montage, bool bInterrupted)
			{
				if (Owner && Owner->GetStateMachine())
				{
					Owner->GetStateMachine()->SetState(ECharacterState::Idle);
				}
			});

			AnimInstance->Montage_SetEndDelegate(ReloadEnded, Owner->ReloadMontage);
		}
	}
}

void UControlComponent::Dash(const FInputActionValue& Value)
{
	if (!bInputEnabled) return;
	
	if (Owner->GetWorldTimerManager().IsTimerActive(DashTimer))	return;

	if (Owner->GetStaminaComponent()->GetCurrentStaminaCount() <= 0)
	{
		OnCoolTime.Broadcast();
		return;
	}
	
	if ( Owner->GetCharacterMovement()->IsFalling())
	{
		FVector ForwardVector = Owner->GetActorForwardVector();
		FVector DashVector = FVector(ForwardVector.X, ForwardVector.Y, 0) * DashPower;

		Owner->LaunchCharacter(DashVector, true, false);
		
		Owner->GetStaminaComponent()->UseStamina();

		if (Owner->GetStaminaComponent()->GetCurrentStaminaCount() == 0)
		{
			LastSkillCharge.Broadcast();
		}

		Owner->GetWorldTimerManager().SetTimer(
		DashTimer,
		this,
		&UControlComponent::DoNothing,
		0.3f,
		false);
	}
}

void UControlComponent::StartJump(const FInputActionValue& Value)
{
	if (!bInputEnabled) return;

	
	
	if (Value.Get<bool>())
	{
		if (Owner->GetSoundComponent())
		{
			Owner->GetSoundComponent()->PlayJumpSound();
		}
		
		Owner->Jump();
	}
}

void UControlComponent::StopJump(const FInputActionValue& Value)
{
	if (!bInputEnabled) return;
	
	if (!Value.Get<bool>())
	{
		Owner->StopJumping();
	}
}

void UControlComponent::SwapWeapon1(const FInputActionValue& Value)
{

	if (!bInputEnabled) return;

	
	UE_LOG(LogTemp,Warning,TEXT("Swap Weapon 1 "));

}

void UControlComponent::SwapWeapon2(const FInputActionValue& Value)
{
	if (!bInputEnabled) return;
	
	UE_LOG(LogTemp,Warning,TEXT("Swap Weapon 2 "));
}

void UControlComponent::DeBug1(const FInputActionValue& Value)
{
	Owner->AddExp(20);

	int Exp = Owner->GetExp();
	
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Add Exp%d"), Exp));

}

void UControlComponent::DeBug2(const FInputActionValue& Value)
{

	UGameplayStatics::ApplyDamage(
					Owner,
					10,
					Owner->GetController(),
					Owner,
					UDamageType::StaticClass()
					);

	int HP = Owner->GetHPComponent()->GetCurrentHP();

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Current HP %d"), HP));
}

//아이템 스피드 증가 함수
void UControlComponent::AddSpeed(float Multiplier)
{
	if (Owner)
	{
		Owner->GetCharacterMovement()->MaxWalkSpeed *= Multiplier;
	}
	UE_LOG(LogTemp,Warning,TEXT("Increase Speed : %f"),Owner->GetCharacterMovement()->MaxWalkSpeed);
}

//아이템 스피드 증가 제거 함수 
void UControlComponent::ResetSpeed()
{
	if (Owner)
	{
		Owner->GetCharacterMovement()->MaxWalkSpeed = MaxSpeed; 
	}
	UE_LOG(LogTemp,Warning,TEXT("Speed Reset : %f"),Owner->GetCharacterMovement()->MaxWalkSpeed);
}

void UControlComponent::HandleCharacterDeath()
{
	DisableInput();
}


