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
#include "Item/ItemBox.h"
#include "Weapons/Pickup_Weapon.h"


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
	UCharacterStateMachine* StateMachine = Owner->GetStateMachine();
	if (!StateMachine || !StateMachine->CanFire()) return;
	
	AWeaponBase* CurrentWeapon = Owner->GetInventoryComponent()->GetCurrentWeapon();
	if (CurrentWeapon && CurrentWeapon->Implements<UWeaponInterface>())
	{
		IWeaponInterface::Execute_PrimaryFire(CurrentWeapon);
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
	
	if (Owner->GetStateMachine()->CanMelee())
	{
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
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	
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

				UGameplayStatics::ApplyDamage(
					Target,
					Owner->GetAttack(),
					Owner->GetController(),
					Owner,
					UDamageType::StaticClass()
					);
				
				if (AMonsterBase* Monster = Cast<AMonsterBase>(Target))
				{
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
	if (!Owner || !Owner->Controller || !bInputEnabled) return;

	UCharacterStateMachine* StateMachine = Owner->GetStateMachine();
	if (!StateMachine) return;

	AWeaponBase* CurrentWeapon = Owner->GetInventoryComponent()->GetCurrentWeapon();
	if (!CurrentWeapon) return;

	
	if (CurrentWeapon->Implements<UWeaponInterface>())
	{
		IWeaponInterface::Execute_Reload(CurrentWeapon);
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
	BeginSwapWeapon(0);
	UE_LOG(LogTemp,Warning,TEXT("Swap Weapon 1 "));
}

void UControlComponent::SwapWeapon2(const FInputActionValue& Value)
{
	BeginSwapWeapon(1);
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

void UControlComponent::Interact(const FInputActionValue& Value)
{
	if (!bInputEnabled||!Owner)return;
	if (!Value.Get<bool>()) return;

	if (UCharacterStateMachine* StateMachine = Owner->GetStateMachine())
	{
		if (!StateMachine->CanSwapping())
		{
			return;
		}
	}

	TArray<AActor*> Overlapping;
	Owner->GetOverlappingActors(Overlapping, APickup_Weapon::StaticClass());

	APickup_Weapon* Best = nullptr;
	float BestDistSq = FLT_MAX;

	for (AActor* Actor : Overlapping)
	{
		if (APickup_Weapon* Pickup = Cast<APickup_Weapon>(Actor))
		{
			const float DistSq = FVector::DistSquared(Pickup->GetActorLocation(), Owner->GetActorLocation());
			if (DistSq < BestDistSq)
			{
				BestDistSq = DistSq;
				Best = Pickup;
			}
		}
	}
	if (Best)
	{
		if (!Best->TryConsume(Owner))
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to Pickup"));
		}
	}
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

void UControlComponent::BeginSwapWeapon(int32 SlotIndex)
{
	if (!bInputEnabled || !Owner) return;

	UCharacterStateMachine* StateMachine = Owner->GetStateMachine();
	UInventoryComponent* Inventory = Owner->GetInventoryComponent();
	if (!StateMachine || !Inventory) return;
	
	if (StateMachine->GetCurrentState() != ECharacterState::Idle) {
		return;
	}
	
	if (Inventory->GetCurrentWeaponIndex() == SlotIndex) return;
	if (!Inventory->HasWeaponInSlot(SlotIndex)) return;
	
	if (AWeaponBase* CurrentWeapon = Inventory->GetCurrentWeapon())
	{
		if (CurrentWeapon->Implements<UWeaponInterface>())
		{
			IWeaponInterface::Execute_StopFire(CurrentWeapon);
		}
		CurrentWeapon->SetActorHiddenInGame(true);
	}

	UAnimMontage* EquipMontage = Inventory->GetEquipMontageForSlot(SlotIndex);
	if (EquipMontage)
	{
		StateMachine->SetState(ECharacterState::Swapping);
		const float Duration = Owner->GetMesh()->GetAnimInstance()->Montage_Play(EquipMontage);

		FOnMontageEnded SwapEndedDelegate;
		SwapEndedDelegate.BindLambda([this, SlotIndex,StateMachine](UAnimMontage* Montage, bool bInterrupted)
		{
			if (StateMachine)
			{
				StateMachine->SetState(ECharacterState::Idle);
			}
			if (Owner && Owner->GetInventoryComponent())
			{
				UInventoryComponent* Inv = Owner->GetInventoryComponent();
				if (Inv->GetCurrentWeaponIndex() != SlotIndex)
				{
					Inv->EquipWeaponAtIndex(SlotIndex);
				}
				
				if (AWeaponBase* Cur = Inv->GetCurrentWeapon())
				{
					Cur->SetActorHiddenInGame(false);
				}
			}
		});
		Owner->GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(SwapEndedDelegate, EquipMontage);

		Owner->SetPendingWeaponSlot(SlotIndex);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Equip Montage"));
		FinishSwapWeapon(SlotIndex);
	}
}

void UControlComponent::FinishSwapWeapon(int32 SlotIndex)
{
	if (!Owner) return;

	UInventoryComponent* Inventory = Owner->GetInventoryComponent();
	if (Inventory)
	{
		Inventory->EquipWeaponAtIndex(SlotIndex);	
	}

	if (Owner->GetStateMachine())
	{
		Owner->GetStateMachine()->SetState(ECharacterState::Idle);
	}
}

void UControlComponent::SetMaxSpeed(float NewMaxSpeed)
{
	MaxSpeed = NewMaxSpeed;
	if (Owner && Owner->GetCharacterMovement())
	{
		Owner->GetCharacterMovement()->MaxWalkSpeed = NewMaxSpeed;
	}
}

