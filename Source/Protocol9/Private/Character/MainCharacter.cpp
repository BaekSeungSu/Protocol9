
#include "Character/MainCharacter.h"
#include "Character/MainPlayerController.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/StaminaComponent.h"
#include "Character/HPComponent.h"
#include "Character/ControlComponent.h"
#include "Character/CharacterStateMachine.h"
#include "Components/SkeletalMeshComponent.h"
#include "Weapons/InventoryComponent.h"
#include "Weapons/WeaponBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "Weapons/WeaponInterface.h"

AMainCharacter::AMainCharacter()
	:Attack(10),
	LevelUpAttack(2),
	Exp(0),
	MaxExp(100),
	CharacterLevel(1)
	
{
	PrimaryActorTick.bCanEverTick = false;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

	StateMachine = CreateDefaultSubobject<UCharacterStateMachine>(TEXT("StateMachine"));

	HPComponent = CreateDefaultSubobject<UHPComponent>(TEXT("HP"));
	StaminaComponent = CreateDefaultSubobject<UStaminaComponent>(TEXT("Stamina"));
	ControlComponent = CreateDefaultSubobject<UControlComponent>(TEXT("Control"));

	BasetAttack = 20.0f;
	LevelUpAttack = 1.2f;
	Attack = BasetAttack;
	CurrentAttack = Attack;

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

void AMainCharacter::AddAttack(float Multiplied)
{
	CurrentAttack *= Multiplied;
	UE_LOG(LogTemp, Warning,TEXT("Increased	My Attack : %f"),CurrentAttack);
}

void AMainCharacter::ResetAttack()
{
	CurrentAttack = Attack;
	UE_LOG(LogTemp, Warning,TEXT("Return My Attack : %f"),CurrentAttack);
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

			if (PlayerController->MeleeAction)
			{
				EnhancedInput->BindAction(
					PlayerController->MeleeAction,
					ETriggerEvent::Started,
					ControlComponent,
					&UControlComponent::Melee
					);
			}

			if (PlayerController->ReloadAction)
			{
				EnhancedInput->BindAction(
					PlayerController->ReloadAction,
					ETriggerEvent::Started,
					ControlComponent,
					&UControlComponent::Reload
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

void AMainCharacter::SetAttack(int NewAttack)
{
	if (NewAttack > 0)
	{
		Attack = NewAttack;
	}
}

void AMainCharacter::SetExp(int NewExp)
{
	if (NewExp > 0)
	{
		Exp = NewExp;
	}
}

void AMainCharacter::SetLevel(int NewLevel)
{
	if (NewLevel > 0)
	{
		CharacterLevel = NewLevel;
	}
}

void AMainCharacter::LevelUp()
{
	if (Exp >= MaxExp)
	{
		CharacterLevel++;

		Attack += LevelUpAttack;
		
		LevelUp();
	}
}
