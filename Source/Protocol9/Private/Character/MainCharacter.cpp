#include "Character/MainCharacter.h"
#include "Character/MainPlayerController.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/StaminaComponent.h"
#include "Character/HPComponent.h"
#include "Character/ControlComponent.h"
#include "Character/CharacterStateMachine.h"
#include "Character/SoundComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Weapons/InventoryComponent.h"
#include "Weapons/WeaponBase.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy/MonsterSpawner.h"
#include "Enemy/MonsterBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "UI/PlayerUIComponent.h"
#include "UI/UWBP_HUD.h"
#include "GameFramework/PlayerController.h"
#include "GameMode/MainGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Weapons/WeaponInterface.h"

AMainCharacter::AMainCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	SituationAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Situation"));
	SituationAudioComponent->SetupAttachment(RootComponent);
	DialogueAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Dialogue"));
	DialogueAudioComponent->SetupAttachment(RootComponent);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	
	StateMachine = CreateDefaultSubobject<UCharacterStateMachine>(TEXT("StateMachine"));

	HPComponent = CreateDefaultSubobject<UHPComponent>(TEXT("HP"));
	StaminaComponent = CreateDefaultSubobject<UStaminaComponent>(TEXT("Stamina"));
	ControlComponent = CreateDefaultSubobject<UControlComponent>(TEXT("Control"));
	SoundComponent = CreateDefaultSubobject<USoundComponent>(TEXT("Sound"));
	
	PlayerUIComponent = CreateDefaultSubobject<UPlayerUIComponent>(TEXT("PlayerUI"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

	InitCharacterInfo();
	
}

void AMainCharacter::InitCharacterInfo()
{
	BasetAttack = 20.0f;
	LevelUpAttack = 1.2f;
	Attack = BasetAttack; 
	CurrentAttack = Attack;
	Exp = 0;
	MaxExp = 100;
	CharacterLevel = 1;
}

void AMainCharacter::EquipDefaultWeapon()
{
	if (!InventoryComponent || !DefaultWeaponClass) return;

	InventoryComponent->AddWeaponToSlot(DefaultWeaponClass, 0, true);
}

void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	//UI 바인딩 : 체력, 경험치, 레벨업, 스태미나 변경 시 HUD 갱신
	if (HPComponent)
	{
		HPComponent->HPChanged.AddDynamic(this, &AMainCharacter::HandleHPChanged);
		ExpChanged.AddDynamic(this, &AMainCharacter::HandleEXPChanged);
		LevelUPEvent.AddDynamic(this, &AMainCharacter::HandleLevelChanged);
	}
	if (StaminaComponent)
	{
		StaminaComponent->StaminaChanged.AddDynamic(this, &AMainCharacter::HandleStaminaChanged);
	}
	// 무기 탄약 변경 이벤트 바인딩
	/*if (InventoryComponent)
	{
		if (AWeaponBase* CurrentWeapon = InventoryComponent->GetCurrentWeapon())
		{
			CurrentWeapon->OnAmmoChanged.AddDynamic(this, &AMainCharacter::HandleAmmoChanged);
		}
	}*/

	HideDefalutMesh();
	
	EquipDefaultWeapon();
	
	AMonsterSpawner* Spawner = Cast<AMonsterSpawner>(UGameplayStatics::GetActorOfClass(GetWorld(), AMonsterSpawner::StaticClass()));
	if (Spawner)
	{
		Spawner->OnMonsterSpawned.AddDynamic(this,&AMainCharacter::SetMonsterDead);
	}
	
}

void AMainCharacter::CacheHUD()
{
	if (AMainGameMode* GM = Cast<AMainGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		CachedHUD = GM->GetHUDWidget();
	}
}

//아이템 공격력 증가 함수
void AMainCharacter::AddAttack(float Multiplied)
{
	CurrentAttack *= Multiplied;
	UE_LOG(LogTemp, Warning,TEXT("Increased	My Attack : %f"),CurrentAttack);
}
//아이템 공격력 증가 리셋 함수
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
        
		SpringArmComponent->SetRelativeLocation(FVector(0, 50, 0));  
		SpringArmComponent->SetRelativeRotation(FRotator(0, -90, 0));
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
	GetMesh()->HideBoneByName(FName("canon_base"), EPhysBodyOp::PBO_None);
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

			if (PlayerController->Interact)
			{
				EnhancedInput->BindAction(
					PlayerController->Interact,
					ETriggerEvent::Started,
					ControlComponent,
					&UControlComponent::Interact);
			}

			if
			(PlayerController->DeBug1)
			{
				EnhancedInput->BindAction(
					PlayerController->DeBug1,
					ETriggerEvent::Started,
					ControlComponent,
					&UControlComponent::DeBug1);
			}

			if (PlayerController->DeBug2)
			{
				EnhancedInput->BindAction(
					PlayerController->DeBug2,
					ETriggerEvent::Started,
					ControlComponent,
					&UControlComponent::DeBug2);
			}

			if (PlayerController->Interaction)
			{
				EnhancedInput->BindAction(
					PlayerController->Interaction,
					ETriggerEvent::Started,
					ControlComponent,
					&UControlComponent::Interact);
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
//몬스터 Exp 관련 이벤트
void AMainCharacter::SetMonsterDead(AMonsterBase* Monster)
{
	if (Monster)
	{
		Monster->OnMonsterDead.AddDynamic(this,&AMainCharacter::OnMonsterDead);
	}
}

void AMainCharacter::OnMonsterDead(AMonsterBase* Monster)
{
	AddExp(Monster->Exp);
}


void AMainCharacter::LevelUp()
{
	if (Exp >= MaxExp)
	{
		
		CharacterLevel++;

		Attack += LevelUpAttack;

		Exp -= MaxExp;

		LevelUPEvent.Broadcast(CharacterLevel);
		ExpChanged.Broadcast(Exp);//UI 경험치바 초기화 
		LevelUp();
	}
}
//UI 파트
void AMainCharacter::HandleInvincibilityEffect()
{
	if (CachedHUD)
	{
		CachedHUD->ShowInvincibilityEffect(true);

		// 3초 후 다시 끄기
		GetWorld()->GetTimerManager().ClearTimer(InvincibilityResetHandle);
		GetWorld()->GetTimerManager().SetTimer(InvincibilityResetHandle, [this]()
		{
			CachedHUD->ShowInvincibilityEffect(false);
		}, 3.f, false);
	}
}

void AMainCharacter::HandleSpeedBoostEffect()
{
	if (CachedHUD)
	{
		CachedHUD->ShowSpeedBoostEffect(true);

		GetWorld()->GetTimerManager().ClearTimer(SpeedBoostResetHandle);
		GetWorld()->GetTimerManager().SetTimer(SpeedBoostResetHandle, [this]()
		{
			CachedHUD->ShowSpeedBoostEffect(false);
		}, 3.f, false);
	}
}

void AMainCharacter::HandleAttackBoostEffect()
{
	if (CachedHUD)
	{
		CachedHUD->ShowAttackBoostEffect(true);

		GetWorld()->GetTimerManager().ClearTimer(AttackBoostResetHandle);
		GetWorld()->GetTimerManager().SetTimer(AttackBoostResetHandle, [this]()
		{
			CachedHUD->ShowAttackBoostEffect(false);
		}, 5.f, false);
	}
}
void AMainCharacter::HandleHPChanged(float CurrentHP)
{
	if (CachedHUD && HPComponent)
	{
		float MaxHP = HPComponent->GetMaxHP();
		CachedHUD->UpdateHPBar(CurrentHP, MaxHP);
	}
}
void AMainCharacter::HandleEXPChanged(int CurrentExp)
{
	if (CachedHUD)
	{
		CachedHUD->UpdateEXPBar(CurrentExp, MaxExp);
	}
}
void AMainCharacter::HandleLevelChanged(int NewLevel)
{
	if (CachedHUD)
	{
		CachedHUD->UpdateLevelText(NewLevel);
	}
}
void AMainCharacter::HandleStaminaChanged(int CurrentStamina)
{
	if (CachedHUD)
	{
		CachedHUD->UpdateStaminaBar(CurrentStamina);
	}
}
/*void AMainCharacter::HandleAmmoChanged(int32 CurrentAmmo, int32 MaxAmmo)
{
	if (CachedHUD)
	{
		CachedHUD->UpdateAmmo(CurrentAmmo, MaxAmmo);
	}
}*/



void AMainCharacter::OnNotify_EquipWeapon()
{
	if (InventoryComponent)
	{
		InventoryComponent->EquipWeaponAtIndex(PendingWeaponSlot);
	}
}
