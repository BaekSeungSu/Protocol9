#include "Weapons/WeaponBase.h"
#include "Components/StaticMeshComponent.h"
#include "Weapons/Data/WeaponData.h"
#include "UI/PlayerUIComponent.h"
#include "Character/MainCharacter.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Character/CharacterStateMachine.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetSimulatePhysics(false);
	SetRootComponent(WeaponMesh);

	CurrentWeaponData = nullptr;

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
	if (bIsReloading || CurrentAmmo == CurrentWeaponData->MagazineSize)
	{
		return;
	}
	if (OwningCharacter && OwningCharacter->GetStateMachine())
	{
		OwningCharacter->GetStateMachine()->SetState(ECharacterState::Reload);
	}

	bIsReloading = true;
	UE_LOG(LogTemp, Warning, TEXT("Reload"));

	UAnimMontage* ReloadMontage = GetReloadMontage();
	if (ReloadMontage)
	{
		const float ReloadDuration = OwningCharacter->GetMesh()->GetAnimInstance()->Montage_Play(ReloadMontage);
		if (ReloadDuration > 0.0f)
		{
			GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &AWeaponBase::FinishReload, ReloadDuration, false);
			UE_LOG(LogTemp, Log, TEXT("Reload timer set for %f seconds based on montage length."), ReloadDuration);
			FOnMontageEnded ReloadMontageEndedDelegate;
			ReloadMontageEndedDelegate.BindUObject(this, &AWeaponBase::OnReloadMontageEnded);
			OwningCharacter->GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(ReloadMontageEndedDelegate, ReloadMontage);
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
	LoadWeaponData();
	
}



void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	LastFireTime = GetWorld()->GetTimeSeconds();
	CurrentAmmo--;
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
		OwningCharacter->GetMesh()->GetAnimInstance()->Montage_Play(CurrentWeaponData->FireMontage);
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
	if (bIsReloading || CurrentAmmo <=0 || CurrentWeaponData == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Can not Fire"));
		return false;
	}

	const float TimeBetweenShots = 60.0f / CurrentWeaponData->FireRate;
	if (GetWorld()->GetTimeSeconds() < LastFireTime + TimeBetweenShots)
	{
		return false;
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
	FVector FireDirection = CalculateHitScanDirection();
	if (FireDirection.IsZero()) return;
	
	FVector TraceStart;
	FRotator UnusedRotation;
	GetOwner()->GetInstigatorController()->GetPlayerViewPoint(TraceStart,UnusedRotation);
	FVector TraceEnd = TraceStart + FireDirection * CurrentWeaponData->Range;
	
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner());
	CollisionParams.AddIgnoredActor(this);

	FHitResult HitResult;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_GameTraceChannel1, CollisionParams);

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

FVector AWeaponBase::CalculateHitScanDirection() const
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
		bIsReloading = false;
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
	CurrentAmmo = CurrentWeaponData->MagazineSize;
	LastFireTime = -100.0f; 
}

void AWeaponBase::FinishReload()
{
	if (!CurrentWeaponData) return;

	CurrentAmmo = CurrentWeaponData->MagazineSize;
	bIsReloading = false;
	UE_LOG(LogTemp, Warning, TEXT("Reload Finish"));
}



