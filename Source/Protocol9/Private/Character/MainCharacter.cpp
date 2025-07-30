#include "Character/MainCharacter.h"
#include "Character/MainPlayerController.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Weapons/InventoryComponent.h"
#include "Weapons/WeaponBase.h"
#include "Weapons/WeaponInterface.h"

AMainCharacter::AMainCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(GetMesh());

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	
}

void AMainCharacter::EquipDefaultWeapon()
{
	if (InventoryComponent && DefaultWeaponClass)
	{
		InventoryComponent->AddWeapon(DefaultWeaponClass);
	}
}

void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetMesh()->HideBoneByName(FName("weapon_r"), EPhysBodyOp::PBO_None);

	EquipDefaultWeapon();
}

void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AMainPlayerController* PlayerController = Cast<AMainPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&AMainCharacter::Move);
			}

			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&AMainCharacter::Look);
			}

			if (PlayerController->FireAction)
			{
				EnhancedInput->BindAction(
					PlayerController->FireAction,
					ETriggerEvent::Started,
					this,
					&AMainCharacter::Fire
					);
			}

			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Triggered,
					this,
					&AMainCharacter::StartJump);
			}

			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Completed,
					this,
					&AMainCharacter::StopJumping);
			}

			if (PlayerController->DashAction)
			{
				EnhancedInput->BindAction(
					PlayerController->DashAction,
					ETriggerEvent::Triggered,
					this,
					&AMainCharacter::Dash);
			}
		}
	}
}

void AMainCharacter::Move(const FInputActionValue& Value)
{
	if (!Controller) return;

	const FVector2D MoveInput = Value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(GetActorForwardVector(), MoveInput.X);
	}

	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
	}
}

void AMainCharacter::Look(const FInputActionValue& Value)
{
	if (!Controller) return;

	const FVector2D LookInput = Value.Get<FVector2D>();

	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(-LookInput.Y);
}

void AMainCharacter::Fire(const FInputActionValue& Value)
{
	if (InventoryComponent)
	{
		AWeaponBase* CurrentWeapon = InventoryComponent->GetCurrentWeapon();

		if (CurrentWeapon && CurrentWeapon->Implements<UWeaponInterface>())
		{
			IWeaponInterface::Execute_PrimaryFire(CurrentWeapon);
		}
	}

}

void AMainCharacter::Dash(const FInputActionValue& Value)
{
}

void AMainCharacter::StartJump(const FInputActionValue& Value)
{
	if (Value.Get<bool>())
	{
		Jump();
	}
}

void AMainCharacter::StopJump(const FInputActionValue& Value)
{
	if (!Value.Get<bool>())
	{
		StopJumping();
	}
}