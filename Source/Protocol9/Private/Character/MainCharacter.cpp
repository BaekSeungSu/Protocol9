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
#include "UI/PlayerUIComponent.h"
#include "Weapons/WeaponInterface.h"

AMainCharacter::AMainCharacter()
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

	PlayerUIComponent = CreateDefaultSubobject<UPlayerUIComponent>(TEXT("PlayerUI"));
	
	BasetAttack = 20.0f;
	LevelUpAttack = 1.2f;
	Attack = BasetAttack;
	CurrentAttack = Attack;
	Exp = 0;
	MaxExp = 100;
	CharacterLevel = 1;

	DeathCameraSocket = TEXT("head");


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

	HideDefalutMesh();
	
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

void AMainCharacter::SetupDeathCamera()
{
	OriginalSpringArmParent = SpringArmComponent->GetAttachParent();
	OriginalSpringArmLocation = SpringArmComponent->GetRelativeLocation();
	OriginalSpringArmRotation = SpringArmComponent->GetRelativeRotation();

	if (GetMesh())
	{
		SpringArmComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		SpringArmComponent->AttachToComponent(
			GetMesh(),
			FAttachmentTransformRules::KeepWorldTransform,
			DeathCameraSocket
		);

		// SpringArmComponent->bEnableCameraLag = true;
		// SpringArmComponent->bEnableCameraRotationLag = true;
		// SpringArmComponent->CameraLagSpeed = 3.0f;
		// SpringArmComponent->CameraRotationLagSpeed = 3.0f;
        
		SpringArmComponent->SetRelativeLocation(FVector(0, 50, 0));  // 측면에서 보기
		SpringArmComponent->SetRelativeRotation(FRotator(0, -90, 0));  // 캐릭터를 향해 보기
	}

}

void AMainCharacter::ResetCameraToDefault()
{
	if (OriginalSpringArmParent)
	{
		SpringArmComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		SpringArmComponent->AttachToComponent(
			OriginalSpringArmParent,
			FAttachmentTransformRules::KeepWorldTransform
		);
		SpringArmComponent->SetRelativeLocation(OriginalSpringArmLocation);
		SpringArmComponent->SetRelativeRotation(OriginalSpringArmRotation);
        
		// SpringArmComponent->bEnableCameraLag = false;
		// SpringArmComponent->bEnableCameraRotationLag = false;
	}

}

float AMainCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (HPComponent)
	{
		HPComponent->HandleDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	}
	
	return ActualDamage;
}

void AMainCharacter::HideDefalutMesh()
{
	GetMesh()->HideBoneByName(FName("weapon_r"), EPhysBodyOp::PBO_None);
	GetMesh()->HideBoneByName(FName("neck_01"), EPhysBodyOp::PBO_None);
	GetMesh()->HideBoneByName(FName("thigh_l"), EPhysBodyOp::PBO_None);
	GetMesh()->HideBoneByName(FName("thigh_r"), EPhysBodyOp::PBO_None);
}

void AMainCharacter::ShowDefalutMesh()
{
	GetMesh()->UnHideBoneByName(FName("neck_01"));
	GetMesh()->UnHideBoneByName(FName("thigh_l"));
	GetMesh()->UnHideBoneByName(FName("thigh_r"));
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
					&UControlComponent::StartFire
					);
			}

			if (PlayerController->FireAction)
			{
				EnhancedInput->BindAction(
					PlayerController->FireAction,
					ETriggerEvent::Completed,
					ControlComponent,
					&UControlComponent::StopFire
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
					ETriggerEvent::Started,
					ControlComponent,
					&UControlComponent::Dash);
			}
			
			if
			(PlayerController->SwapWeapon1)
			{
				EnhancedInput->BindAction(
					PlayerController->SwapWeapon1,
					ETriggerEvent::Started,
					ControlComponent,
					&UControlComponent::SwapWeapon1);
			}

			if (PlayerController->SwapWeapon2)
			{
				EnhancedInput->BindAction(
					PlayerController->SwapWeapon2,
					ETriggerEvent::Started,
					ControlComponent,
					&UControlComponent::SwapWeapon2);
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

void AMainCharacter::AddExp(int NewExp)
{
	Exp += NewExp;
	UE_LOG(LogTemp, Display, TEXT("AddExp %d"), NewExp);
	ExpChanged.Broadcast(Exp);
	
	if (Exp>=MaxExp)
	{
		LevelUp();
	}
	
}


void AMainCharacter::LevelUp()
{
	if (Exp >= MaxExp)
	{
		CharacterLevel++;

		Attack += LevelUpAttack;

		Exp -= MaxExp;

		LevelUPEvent.Broadcast(CharacterLevel);
		
		LevelUp();
	}
}

