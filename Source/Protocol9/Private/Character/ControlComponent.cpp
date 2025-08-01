#include "Character/ControlComponent.h"
#include "Character/MainCharacter.h"
#include "Character/StaminaComponent.h"
#include "Character/CharacterStateMachine.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Weapons/InventoryComponent.h"
#include "Weapons/WeaponBase.h"


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
	}
}

void UControlComponent::Move(const FInputActionValue& Value)
{
	if (!Owner->Controller) return;

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

	const FVector2D LookInput = Value.Get<FVector2D>();

	Owner->AddControllerYawInput(LookInput.X);

	Owner->AddControllerPitchInput(-LookInput.Y);
}

void UControlComponent::Fire(const FInputActionValue& Value)
{
	//AWeaponBase* CurrentWeapon = Owner->GetInventoryComponent()->GetCurrentWeapon();
	
	if (!Owner->Controller) return;

	if (Owner->GetStateMachine()->CanFire()) return;

	// if (CurrentWeapon && CurrentWeapon->Implements<UWeaponInterface>())
	// {
	// 	IWeaponInterface::Execute_PrimaryFire(CurrentWeapon);
	// }
	
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

void UControlComponent::Melee(const FInputActionValue& Value)
{
	if (!Owner->Controller) return;
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Melee"));
	}
}

void UControlComponent::Reload(const FInputActionValue& Value)
{
	if (!Owner->Controller) return;

	if (Owner->GetStateMachine()->CanFire()) return;

	Owner->GetStateMachine()->SetState(ECharacterState::Reload);

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, (TEXT("Reload!")));

	UAnimInstance* AnimInstance = Owner->GetMesh()->GetAnimInstance();
	{
		if (AnimInstance && Owner->FireMontage)
		{
			float duration = AnimInstance->Montage_Play(Owner->ReloadMontage);
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("%f"), duration));

			// FOnMontageEnded ReloadEnded;
			// ReloadEnded.BindLambda([this](UAnimMontage* Montage, bool bInterrupted)
			// {
			// 	if (Owner && Owner->GetStateMachine())
			// 	{
			// 		Owner->GetStateMachine()->SetState(ECharacterState::Idle);
			// 	}
			// });

		}
	}
}

void UControlComponent::Dash(const FInputActionValue& Value)
{
	if (Owner->GetWorldTimerManager().IsTimerActive(DashTimer))
	{
		return;
	}
	
	if ( Owner->GetCharacterMovement()->IsFalling() && Owner->GetStaminaComponent()->GetCurrentStaminaCount() > 0)
	{
		FVector ForwardVector = Owner->GetActorForwardVector();
		FVector DashVector = FVector(ForwardVector.X, ForwardVector.Y, 0) * DashPower;

		Owner->LaunchCharacter(DashVector, true, false);
		
		Owner->GetStaminaComponent()->UseStamina();

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
	if (Value.Get<bool>())
	{
		Owner->Jump();
	}
}

void UControlComponent::StopJump(const FInputActionValue& Value)
{
	if (!Value.Get<bool>())
	{
		Owner->StopJumping();
	}
}


void UControlComponent::AddSpeed(float Multiplier)
{
	if (Owner)
	{
		Owner->GetCharacterMovement()->MaxWalkSpeed *= Multiplier;
	}
	UE_LOG(LogTemp,Warning,TEXT("Increase Speed : %f"),Owner->GetCharacterMovement()->MaxWalkSpeed);
}

void UControlComponent::ResetSpeed()
{
	if (Owner)
	{
		Owner->GetCharacterMovement()->MaxWalkSpeed = MaxSpeed; 
	}
	UE_LOG(LogTemp,Warning,TEXT("Speed Reset : %f"),Owner->GetCharacterMovement()->MaxWalkSpeed);
}
