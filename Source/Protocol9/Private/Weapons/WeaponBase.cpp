#include "Weapons/WeaponBase.h"
#include "Components/StaticMeshComponent.h"
#include "Weapons/Data/WeaponData.h"
#include "UI/PlayerUIComponent.h"
#include "Character/MainCharacter.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Character/StaminaComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Character/CharacterStateMachine.h"
#include "GameMode/MainGameMode.h" //UI
#include "Weapons/BaseProjectile.h"



AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetSimulatePhysics(false);
	SetRootComponent(WeaponMesh);

	CurrentWeaponData = nullptr;
	bIsFullAuto = false;
}

void AWeaponBase::PrimaryFire_Implementation()
{
	FireAction();
}

void AWeaponBase::StopFire_Implementation()
{
	
}

void AWeaponBase::Reload_Implementation()
{
	if (!CurrentWeaponData || CurrentAmmo >= CurrentWeaponData->MagazineSize) return;
	if (!OwningCharacter) return;

	if (GetWorld()->GetTimerManager().IsTimerActive(ReloadTimerHandle)) return;
	
	UCharacterStateMachine* StateMachine = OwningCharacter->GetStateMachine();
	if (!StateMachine) return;

	if (StateMachine->GetCurrentState() != ECharacterState::Idle) return;

	StopFire_Implementation();
	
	StateMachine->SetState(ECharacterState::Reload);
	UE_LOG(LogTemp, Warning, TEXT("Reload"));

	UAnimMontage* ReloadMontage = GetReloadMontage();
	if (ReloadMontage)
	{
		UAnimInstance* AnimInstance = OwningCharacter->GetMesh()->GetAnimInstance();
		const float ReloadDuration = OwningCharacter->GetMesh()->GetAnimInstance()->Montage_Play(ReloadMontage);
		if (ReloadDuration > 0.0f)
		{
			GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &AWeaponBase::FinishReload, ReloadDuration, false);
			
			FOnMontageEnded ReloadEndedDelegate;
			ReloadEndedDelegate.BindUObject(this, &AWeaponBase::OnReloadMontageEnded);
			AnimInstance->Montage_SetEndDelegate(ReloadEndedDelegate, ReloadMontage);
		}
	}
}


void AWeaponBase::SetOwningCharacter(AMainCharacter* NewOwner)
{
	OwningCharacter = NewOwner;
}

UAnimMontage* AWeaponBase::GetFireMontage() const
{
	if (CurrentWeaponData)
	{
		return CurrentWeaponData->FireMontage;
	}
	return nullptr;
}

UAnimMontage* AWeaponBase::GetReloadMontage() const
{
	if (CurrentWeaponData)
	{
		return CurrentWeaponData->ReloadMontage;
	}
	return nullptr;
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}



void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponBase::AttachToOwnerSocket()
{
	if (!OwningCharacter || !CurrentWeaponData) return;

	AttachToComponent(OwningCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));

	SetActorRelativeLocation(CurrentWeaponData->AttachLocationOffset);
	SetActorRelativeRotation(CurrentWeaponData->AttachRotationOffset);
	SetActorRelativeScale3D(CurrentWeaponData->AttachScale);
}

void AWeaponBase::FireAction()
{
	if (!CanFire())
	{
		if (CurrentAmmo <= 0)
		{
			IWeaponInterface::Execute_Reload(this);
		}
		return;
	}
	if (OwningCharacter && OwningCharacter->GetStateMachine())
	{
		OwningCharacter->GetStateMachine()->SetState(ECharacterState::Fire);
	}
	
	LastFireTime = GetWorld()->GetTimeSeconds();
	CurrentAmmo--;
	// UI 추가
	if (AMainGameMode* GM = Cast<AMainGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->UpdateAmmoUI(CurrentAmmo, 0, false);
	}
	ApplyRecoil();
	
	
	if (CurrentWeaponData->MuzzleFlash)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(CurrentWeaponData->MuzzleFlash, WeaponMesh, TEXT("MuzzleSocket"), FVector(0.f), FRotator(0.f), EAttachLocation::KeepRelativeOffset, true);
	}
	if (CurrentWeaponData->FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), CurrentWeaponData->FireSound, GetActorLocation());
	}

	if (OwningCharacter && CurrentWeaponData ->FireMontage)
	{
		UAnimInstance* AnimInstance = OwningCharacter->GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			const float Duration = AnimInstance->Montage_Play(CurrentWeaponData->FireMontage);
			FOnMontageEnded FireEndedDelegate;
			FireEndedDelegate.BindLambda([this](UAnimMontage* Montage, bool bInterrupted)
			{
				if (OwningCharacter && OwningCharacter->GetStateMachine())
				{
					OwningCharacter->GetStateMachine()->SetState(ECharacterState::Idle);
				}
			});
			AnimInstance->Montage_SetEndDelegate(FireEndedDelegate, CurrentWeaponData->FireMontage);
		}
	}
	
	
	if (CurrentWeaponData)
	{
		if (CurrentWeaponData->FireType == EFireType::HitScan)
		{
			FireHitScan();
		}
		else if (CurrentWeaponData->FireType == EFireType::Projectile)
		{
			FireProjectile();
		}
	}
	
}

bool AWeaponBase::CanFire() const
{
	if (CurrentAmmo <=0 || CurrentWeaponData == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Can not Fire"));
		return false;
	}

	if (!bIsFullAuto)
	{
		const float TimeBetweenShots = 60.0f / CurrentWeaponData->FireRate;
		if (GetWorld()->GetTimeSeconds() < LastFireTime + TimeBetweenShots)
		{
			return false;
		}	
	}
	
	
	return true;
}

void AWeaponBase::ApplyRecoil()
{
	APlayerController* PlayerController = GetOwner()->GetInstigatorController<APlayerController>();
	if (!PlayerController || !CurrentWeaponData) return;

	const float Pitch = -CurrentWeaponData->RecoilPitch;
	const float Yaw = FMath::RandRange(-CurrentWeaponData->RecoilYaw, CurrentWeaponData->RecoilYaw);
	PlayerController->AddPitchInput(Pitch);
	PlayerController->AddYawInput(Yaw);
	
	if (CurrentWeaponData->RecoilCameraShake)
	{
		PlayerController->ClientStartCameraShake(CurrentWeaponData->RecoilCameraShake);
	}
		

	AMainCharacter* OwnerCharacter = Cast<AMainCharacter>(GetOwner());
	if (OwnerCharacter)
	{
		UPlayerUIComponent* UIComponent = OwnerCharacter->FindComponentByClass<UPlayerUIComponent>();
		if (UIComponent)
		{
			float CurrentAimSize = UIComponent->GetAimSize();
			UIComponent->SetAimSize(CurrentAimSize + CurrentWeaponData->CrosshairRecoilAmount);
		}
	}
}




void AWeaponBase::FireHitScan()
{
	FVector FireDirection = CalculateFireDirection();
	if (FireDirection.IsZero()) return;
	
	FVector TraceStart;
	FRotator UnusedRotation;
	GetOwner()->GetInstigatorController()->GetPlayerViewPoint(TraceStart,UnusedRotation);
	FVector TraceEnd = TraceStart + FireDirection * CurrentWeaponData->Range;
	
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner());
	CollisionParams.AddIgnoredActor(this);

	FHitResult HitResult;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Pawn, CollisionParams);

	FVector MuzzleLocation = WeaponMesh->GetSocketLocation(TEXT("MuzzleSocket"));
	
	if (bHit)
	{
		ProcessHit(HitResult,FireDirection);
	}
	else
	{
		DrawDebugLine(GetWorld(), MuzzleLocation, TraceEnd, FColor::Red, false, 2.0f, 0, 2.0f);
		UE_LOG(LogTemp, Error, TEXT("Miss"));
	}
	
}

FVector AWeaponBase::CalculateFireDirection() const
{
	AController* OwnerController = GetOwner()->GetInstigatorController();
	AMainCharacter* OwnerCharacter = Cast<AMainCharacter>(GetOwner());
	if (!OwnerController || !OwnerCharacter || !CurrentWeaponData)
	{
		return FVector::ZeroVector;
	}
	
	FVector EyeLocation;
	FRotator EyeRotation;
	OwnerController->GetPlayerViewPoint(EyeLocation, EyeRotation);
	FVector FireDirection = EyeRotation.Vector();

	float BaseSpreadAngle = CurrentWeaponData->SpreadAngle;
	float DynamicSpread = 0.0f;
	UPlayerUIComponent* UIComponent = OwnerCharacter->FindComponentByClass<UPlayerUIComponent>();
	if (UIComponent)
	{
		DynamicSpread = UIComponent->GetAimSize() * 0.01f;
	}
	float FinalSpreadAngle = BaseSpreadAngle + DynamicSpread;

	if (FinalSpreadAngle > 0.0f)
	{
		const float ConeHalfAngle = FMath::DegreesToRadians(FinalSpreadAngle * 0.5f);
		FireDirection = FMath::VRandCone(FireDirection, ConeHalfAngle);
	}
	
	return FireDirection;
}

void AWeaponBase::FireProjectile()
{
	if (!CurrentWeaponData || !CurrentWeaponData->ProjectileClass) return;

	
	APlayerController* PlayerController = GetOwner()->GetInstigatorController<APlayerController>();
	FVector EyeLocation;
	FRotator EyeRotation;
	PlayerController->GetPlayerViewPoint(EyeLocation, EyeRotation);
	FVector FireDirection = CalculateFireDirection();

	FVector TraceEnd = EyeLocation + FireDirection * CurrentWeaponData->Range;
	FHitResult HitResult;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, EyeLocation, TraceEnd, ECC_Pawn))
	{
		TraceEnd = HitResult.ImpactPoint;
	}

	
	
	FVector MuzzleLocation = WeaponMesh->GetSocketLocation(TEXT("MuzzleSocket"));
	FVector ProjectileDir = (TraceEnd - MuzzleLocation).GetSafeNormal();
	FRotator ProjectileRotation = ProjectileDir.Rotation();
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	
	ABaseProjectile* SpawnedProjectile = GetWorld()->SpawnActor<ABaseProjectile>(CurrentWeaponData->ProjectileClass, MuzzleLocation, ProjectileRotation, SpawnParams);
	if (SpawnedProjectile)
	{
		SpawnedProjectile->SetDamage(CurrentWeaponData->Damage);
		SpawnedProjectile->FireInDirection(FireDirection);
	}
	
}

void AWeaponBase::ProcessHit(const FHitResult& HitResult, const FVector& ShotDirection)
{
	AActor* HitActor = HitResult.GetActor();
	if (HitActor)
	{
		AController* OwnerController = GetOwner()->GetInstigatorController();
		UGameplayStatics::ApplyPointDamage(HitActor, CurrentWeaponData->Damage, ShotDirection, HitResult, OwnerController, this, UDamageType::StaticClass());
	}

	if (CurrentWeaponData->HitParticle)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), CurrentWeaponData->HitParticle, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
	}
}

void AWeaponBase::OnReloadMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (bInterrupted)
	{
		GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
	}

	if (OwningCharacter && OwningCharacter->GetStateMachine())
	{
		OwningCharacter->GetStateMachine()->SetState(ECharacterState::Idle);
	}
}

void AWeaponBase::LoadWeaponData()
{
	if (!WeaponDataTable) return;
	if (WeaponDataRowName.IsNone()) return;

	CurrentWeaponData = WeaponDataTable->FindRow<FWeaponData>(WeaponDataRowName, TEXT(""));
	if (!CurrentWeaponData) return; 
	CurrentAmmo = CurrentWeaponData->MagazineSize;
	LastFireTime = -100.0f;
	// UI
	if (AMainGameMode* GM = Cast<AMainGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->SetWeaponIconUI(UIIcon);              // 현재 무기 아이콘
		GM->UpdateAmmoUI(CurrentAmmo, 0, false);  // 탄약 텍스트(예비탄 없으면 0/false)
	}
}

void AWeaponBase::FinishReload()
{
	if (!CurrentWeaponData) return;

	CurrentAmmo = CurrentWeaponData->MagazineSize;
	UE_LOG(LogTemp, Warning, TEXT("Reload Finish"));
	// UI 추가
	if (AMainGameMode* GM = Cast<AMainGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->UpdateAmmoUI(CurrentAmmo, 0, false);
	}
}

void AWeaponBase::CancelReload()
{
	if (OwningCharacter && OwningCharacter->GetStateMachine()->GetCurrentState() == ECharacterState::Reload)
	{
		GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
		OwningCharacter->GetMesh()->GetAnimInstance()->Montage_Stop(0.1f, GetReloadMontage());
		OwningCharacter->GetStateMachine()->SetState(ECharacterState::Idle);
	}
}

void AWeaponBase::CancelReload()
{
	if (OwningCharacter && OwningCharacter->GetStateMachine()->GetCurrentState() == ECharacterState::Reload)
	{
		GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
		OwningCharacter->GetMesh()->GetAnimInstance()->Montage_Stop(0.1f, GetReloadMontage());
		OwningCharacter->GetStateMachine()->SetState(ECharacterState::Idle);
	}
}

void AWeaponBase::CancelReload()
{
	if (OwningCharacter && OwningCharacter->GetStateMachine()->GetCurrentState() == ECharacterState::Reload)
	{
		GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
		OwningCharacter->GetMesh()->GetAnimInstance()->Montage_Stop(0.1f, GetReloadMontage());
		OwningCharacter->GetStateMachine()->SetState(ECharacterState::Idle);
	}
}

void AWeaponBase::CancelReload()
{
	if (OwningCharacter && OwningCharacter->GetStateMachine()->GetCurrentState() == ECharacterState::Reload)
	{
		GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
		OwningCharacter->GetMesh()->GetAnimInstance()->Montage_Stop(0.1f, GetReloadMontage());
		OwningCharacter->GetStateMachine()->SetState(ECharacterState::Idle);
	}
}



