
#include "Character/MainCharacter.h"
#include "Character/MainPlayerController.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/StaminaComponent.h"
#include "Character/HPComponent.h"
#include "Character/ControlComponent.h"
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

	HPComponent = CreateDefaultSubobject<UHPComponent>(TEXT("HP"));
	StaminaComponent = CreateDefaultSubobject<UStaminaComponent>(TEXT("Stamina"));
	ControlComponent = CreateDefaultSubobject<UControlComponent>(TEXT("Control"));

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
	GetMesh()->HideBoneByName(FName("neck_01"), EPhysBodyOp::PBO_None);
	GetMesh()->HideBoneByName(FName("thigh_l"), EPhysBodyOp::PBO_None);
	GetMesh()->HideBoneByName(FName("thigh_r"), EPhysBodyOp::PBO_None);

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
					ControlComponent,
					&UControlComponent::Move);
			}

			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					ControlComponent,
					&UControlComponent::Look);
			}

			if (PlayerController->FireAction)
			{
				EnhancedInput->BindAction(
					PlayerController->FireAction,
					ETriggerEvent::Started,
					ControlComponent,
					&UControlComponent::Fire
					);
			}

			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Started,
					ControlComponent,
					&UControlComponent::StartJump);
			}

			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Completed,
					ControlComponent,
					&UControlComponent::StopJump);
			}

			if (PlayerController->DashAction)
			{
				EnhancedInput->BindAction(
					PlayerController->DashAction,
					ETriggerEvent::Triggered,
					ControlComponent,
					&UControlComponent::Dash);
			}
		}
	}
}